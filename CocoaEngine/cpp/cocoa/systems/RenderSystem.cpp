#include "externalLibs.h"

#include "cocoa/util/Log.h"
#include "cocoa/systems/RenderSystem.h"
#include "cocoa/core/Application.h"
#include "cocoa/components/components.h"
#include "cocoa/commands/ICommand.h"
#include "cocoa/util/CMath.h"
#include "cocoa/util/DynamicArray.h"

#include <nlohmann/json.hpp>

namespace Cocoa
{
	namespace RenderSystem
	{
		// Internal Variables
		static Handle<Shader> m_Shader = Handle<Shader>();
		static Handle<Shader> m_SpriteShader = Handle<Shader>();
		static Handle<Shader> m_FontShader = Handle<Shader>();
		static Framebuffer m_MainFramebuffer = Framebuffer();

		static int m_TexSlots[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
		static const int MAX_BATCH_SIZE = 1000;

		static DynamicArrayData<RenderBatchData> m_Batches;
		static Camera* m_Camera;

		void Init(Scene* scene)
		{
			m_Camera = scene->GetCamera();
			m_MainFramebuffer = Framebuffer(3840, 2160);
			m_Batches = DynamicArray::Create<RenderBatchData>(1);
			m_SpriteShader = AssetManager::LoadShaderFromFile(CPath(Settings::General::s_EngineAssetsPath + "shaders/SpriteRenderer.glsl"), true);
			m_FontShader = AssetManager::LoadShaderFromFile(CPath(Settings::General::s_EngineAssetsPath + "shaders/FontRenderer.glsl"), true);
			AssetManager::LoadShaderFromFile(CPath(Settings::General::s_EngineAssetsPath + "shaders/Picking.glsl"), true);
		}

		void Destroy()
		{
			m_MainFramebuffer.Delete();
			for (int i = 0; i < m_Batches.m_NumElements; i++)
			{
				RenderBatchData& data = DynamicArray::Get<RenderBatchData>(m_Batches, i);
				RenderBatch::Free(data);
			}
			DynamicArray::Free<RenderBatchData>(m_Batches);
		}

		void UploadUniform1ui(const char* name, uint32 val)
		{
			const Shader& shader = AssetManager::GetShader(m_Shader);
			if (!shader.IsNull())
			{
				shader.UploadUInt(name, val);
			}
		}

		void BindShader(Handle<Shader> shader)
		{
			m_Shader = shader;
		}

		void AddEntity(const TransformData& transform, const SpriteRenderer& spr)
		{
			const Sprite& sprite = spr.m_Sprite;
			bool wasAdded = false;
			for (int i=0; i < m_Batches.m_NumElements; i++)
			{
				RenderBatchData& batch = DynamicArray::Get<RenderBatchData>(m_Batches, i);
				if (RenderBatch::HasRoom(batch) && spr.m_ZIndex == batch.ZIndex && batch.BatchShader == m_SpriteShader)
				{
					Handle<Texture> tex = sprite.m_Texture;
					if (!tex || RenderBatch::HasTexture(batch, tex) || RenderBatch::HasTextureRoom(batch))
					{
						RenderBatch::Add(batch, transform, spr);
						wasAdded = true;
						break;
					}
				}
			}

			if (!wasAdded)
			{
				RenderBatchData newBatch = RenderBatch::CreateRenderBatch(MAX_BATCH_SIZE, spr.m_ZIndex, m_SpriteShader);
				RenderBatch::Start(newBatch);
				RenderBatch::Add(newBatch, transform, spr);
				DynamicArray::Add(m_Batches, newBatch);
				std::sort(DynamicArray::Begin<RenderBatchData>(m_Batches), DynamicArray::End<RenderBatchData>(m_Batches), RenderBatch::Compare);
			}
		}

		void AddEntity(const TransformData& transform, const FontRenderer& fontRenderer)
		{
			const Font& font = AssetManager::GetFont(fontRenderer.m_Font.m_AssetId);
			bool wasAdded = false;
			for (int i=0; i < m_Batches.m_NumElements; i++)
			{
				RenderBatchData& batch = DynamicArray::Get<RenderBatchData>(m_Batches, i);
				if (RenderBatch::HasRoom(batch, fontRenderer) && fontRenderer.m_ZIndex == batch.ZIndex && batch.BatchShader == m_FontShader)
				{
					Handle<Texture> tex = font.m_FontTexture;
					if (!tex || RenderBatch::HasTexture(batch, tex) || RenderBatch::HasTextureRoom(batch))
					{
						RenderBatch::Add(batch, transform, fontRenderer);
						wasAdded = true;
						break;
					}
				}
			}

			if (!wasAdded)
			{
				RenderBatchData newBatch = RenderBatch::CreateRenderBatch(MAX_BATCH_SIZE, fontRenderer.m_ZIndex, m_FontShader);
				RenderBatch::Start(newBatch);
				RenderBatch::Add(newBatch, transform, fontRenderer);
				DynamicArray::Add<RenderBatchData>(m_Batches, newBatch);
				std::sort(DynamicArray::Begin<RenderBatchData>(m_Batches), DynamicArray::End<RenderBatchData>(m_Batches), RenderBatch::Compare);
			}
		}

		void Render(Scene* scene)
		{
			scene->GetRegistry().group<SpriteRenderer>(entt::get<TransformData>).each([](auto entity, auto& spr, auto& transform)
				{
					AddEntity(transform, spr);
				});

			scene->GetRegistry().group<FontRenderer>(entt::get<TransformData>).each([](auto entity, auto& fontRenderer, auto& transform)
				{
					AddEntity(transform, fontRenderer);
				});

			bool useShader = m_Shader.IsNull();
			if (!useShader)
			{
				const Shader& shader = AssetManager::GetShader(m_Shader.m_AssetId);
				Log::Assert(!shader.IsNull(), "Cannot render with a null shader");
				shader.Bind();
				shader.UploadMat4("uProjection", m_Camera->GetOrthoProjection());
				shader.UploadMat4("uView", m_Camera->GetOrthoView());
				shader.UploadIntArray("uTextures[0]", 16, m_TexSlots);
			}

			for (int i=0; i < m_Batches.m_NumElements; i++)
			{
				RenderBatchData& batch = DynamicArray::Get<RenderBatchData>(m_Batches, i);
				if (useShader)
				{
					Log::Assert(!batch.BatchShader.IsNull(), "Cannot render with a null shader.");
					const Shader& shader = AssetManager::GetShader(batch.BatchShader.m_AssetId);
					shader.Bind();
					shader.UploadMat4("uProjection", m_Camera->GetOrthoProjection());
					shader.UploadMat4("uView", m_Camera->GetOrthoView());
					shader.UploadIntArray("uTextures[0]", 16, m_TexSlots);
				}

				RenderBatch::Render(batch);
				RenderBatch::Clear(batch);
			}
		}

		const Framebuffer& GetMainFramebuffer()
		{
			return m_MainFramebuffer;
		}

		void Serialize(json& j, Entity entity, const SpriteRenderer& spriteRenderer)
		{
			json color = CMath::Serialize("Color", spriteRenderer.m_Color);
			json assetId = { "AssetId", (uint32)std::numeric_limits<uint32>::max() };
			json zIndex = { "ZIndex", spriteRenderer.m_ZIndex };
			if (spriteRenderer.m_Sprite.m_Texture)
			{
				assetId = { "AssetId", spriteRenderer.m_Sprite.m_Texture.m_AssetId };
			}

			int size = j["Components"].size();
			j["Components"][size] = {
				{"SpriteRenderer", {
					{"Entity", entity.GetID()},
					assetId,
					zIndex,
					color
				}}
			};
		}

		void DeserializeSpriteRenderer(json& j, Entity entity)
		{
			SpriteRenderer spriteRenderer;
			spriteRenderer.m_Color = CMath::DeserializeVec4(j["SpriteRenderer"]["Color"]);
			if (j["SpriteRenderer"].contains("AssetId"))
			{
				if (j["SpriteRenderer"]["AssetId"] != std::numeric_limits<uint32>::max())
				{
					spriteRenderer.m_Sprite.m_Texture = Handle<Texture>(j["SpriteRenderer"]["AssetId"]);
				}
			}

			if (j["SpriteRenderer"].contains("ZIndex"))
			{
				spriteRenderer.m_ZIndex = j["SpriteRenderer"]["ZIndex"];
			}
			entity.AddComponent<SpriteRenderer>(spriteRenderer);
		}

		void Serialize(json& j, Entity entity, const FontRenderer& fontRenderer)
		{
			json color = CMath::Serialize("Color", fontRenderer.m_Color);
			json assetId = { "AssetId", (uint32)std::numeric_limits<uint32>::max() };
			json zIndex = { "ZIndex", fontRenderer.m_ZIndex };
			json text = { "Text", fontRenderer.text };
			json fontSize = { "FontSize", fontRenderer.fontSize };
			if (fontRenderer.m_Font)
			{
				assetId = { "AssetId", fontRenderer.m_Font.m_AssetId };
			}

			int size = j["Components"].size();
			j["Components"][size] = {
				{"FontRenderer", {
					{"Entity", entity.GetID()},
					assetId,
					zIndex,
					color,
					text,
					fontSize
				}}
			};
		}

		void DeserializeFontRenderer(json& j, Entity entity)
		{
			FontRenderer fontRenderer;
			fontRenderer.m_Color = CMath::DeserializeVec4(j["FontRenderer"]["Color"]);
			if (j["FontRenderer"].contains("AssetId"))
			{
				if (j["FontRenderer"]["AssetId"] != std::numeric_limits<uint32>::max())
				{
					fontRenderer.m_Font = Handle<Font>(j["FontRenderer"]["AssetId"]);
				}
			}

			if (j["FontRenderer"].contains("ZIndex"))
			{
				fontRenderer.m_ZIndex = j["FontRenderer"]["ZIndex"];
			}

			if (j["FontRenderer"].contains("Text"))
			{
				fontRenderer.text = j["FontRenderer"]["Text"];
			}

			if (j["FontRenderer"].contains("FontSize"))
			{
				fontRenderer.fontSize = j["FontRenderer"]["FontSize"];
			}
			entity.AddComponent<FontRenderer>(fontRenderer);
		}
	}
}