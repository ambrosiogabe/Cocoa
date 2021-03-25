#include "externalLibs.h"

#include "cocoa/util/Log.h"
#include "cocoa/systems/RenderSystem.h"
#include "cocoa/core/Application.h"
#include "cocoa/core/AssetManager.h"
#include "cocoa/commands/ICommand.h"
#include "cocoa/util/CMath.h"
#include "cocoa/util/DynamicArray.h"
#include "cocoa/renderer/DebugDraw.h"
#include "cocoa/renderer/CameraStruct.h"

#include <nlohmann/json.hpp>

namespace Cocoa
{
	namespace RenderSystem
	{
		// Internal Variables
		static Handle<Shader> m_SpriteShader = Handle<Shader>();
		static Handle<Shader> m_FontShader = Handle<Shader>();

		static int m_TexSlots[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
		static const int MAX_BATCH_SIZE = 1000;

		static DynamicArray<RenderBatchData> m_Batches;

		void Init()
		{
			m_Batches = NDynamicArray::Create<RenderBatchData>(1);

			CPath spriteShaderPath = Settings::General::s_EngineAssetsPath;
			NCPath::Join(spriteShaderPath, NCPath::CreatePath("shaders/SpriteRenderer.glsl"));
			m_SpriteShader = AssetManager::LoadShaderFromFile(spriteShaderPath, true);
			CPath fontShaderPath = Settings::General::s_EngineAssetsPath;
			NCPath::Join(fontShaderPath, NCPath::CreatePath("shaders/FontRenderer.glsl"));
			m_FontShader = AssetManager::LoadShaderFromFile(fontShaderPath, true);
			CPath pickingShaderPath = Settings::General::s_EngineAssetsPath;
			NCPath::Join(pickingShaderPath, NCPath::CreatePath("shaders/Picking.glsl"));
			AssetManager::LoadShaderFromFile(pickingShaderPath, true);
		}

		void Destroy()
		{
			for (int i = 0; i < m_Batches.m_NumElements; i++)
			{
				RenderBatchData& data = NDynamicArray::Get<RenderBatchData>(m_Batches, i);
				RenderBatch::Free(data);
			}
			NDynamicArray::Free<RenderBatchData>(m_Batches);
		}

		void AddEntity(const TransformData& transform, const SpriteRenderer& spr)
		{
			const Sprite& sprite = spr.m_Sprite;
			bool wasAdded = false;
			for (int i = 0; i < m_Batches.m_NumElements; i++)
			{
				RenderBatchData& batch = NDynamicArray::Get<RenderBatchData>(m_Batches, i);
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
				NDynamicArray::Add(m_Batches, newBatch);
				std::sort(NDynamicArray::Begin<RenderBatchData>(m_Batches), NDynamicArray::End<RenderBatchData>(m_Batches), RenderBatch::Compare);
			}
		}

		void AddEntity(const TransformData& transform, const FontRenderer& fontRenderer)
		{
			const Font& font = AssetManager::GetFont(fontRenderer.m_Font.m_AssetId);
			bool wasAdded = false;
			for (int i = 0; i < m_Batches.m_NumElements; i++)
			{
				RenderBatchData& batch = NDynamicArray::Get<RenderBatchData>(m_Batches, i);
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
				NDynamicArray::Add<RenderBatchData>(m_Batches, newBatch);
				std::sort(NDynamicArray::Begin<RenderBatchData>(m_Batches), NDynamicArray::End<RenderBatchData>(m_Batches), RenderBatch::Compare);
			}
		}

		void Render(const SceneData& scene)
		{
			scene.Registry.view<const SpriteRenderer, const TransformData>().each([](auto entity, const auto& spriteRenderer, const auto& transform)
				{
					AddEntity(transform, spriteRenderer);
				});

			scene.Registry.view<const FontRenderer, const TransformData>().each([](auto entity, const auto& fontRenderer, const auto& transform)
				{
					AddEntity(transform, fontRenderer);
				});

			// TODO: This is super broken
			auto view = scene.Registry.view<const Camera>();
			for (auto& rawEntity : view)
			{
				const Camera& camera = scene.Registry.get<Camera>(rawEntity);
				NFramebuffer::Bind(camera.Framebuffer);

				glEnable(GL_BLEND); // TODO: This should be encapsulated within the camera somehow
				glViewport(0, 0, 3840, 2160); // TODO: This should be encapsulated within the camera somehow
				glClearColor(0.45f, 0.55f, 0.6f, 1.0f); // TODO: This should be encapsulated within the camera somehow
				glClear(GL_COLOR_BUFFER_BIT); // TODO: This should be encapsulated within the camera somehow
				//NFramebuffer::ClearColorAttachmentUint32(camera.Framebuffer, 1, (uint32)-1); // TODO: This should be encapsulated within the camera somehow
				//RenderSystem::UploadUniform1ui("uActiveEntityID", InspectorWindow::GetActiveEntity().GetID() + 1);

				DebugDraw::DrawBottomBatches(camera);

				for (int i = 0; i < m_Batches.m_NumElements; i++)
				{
					RenderBatchData& batch = NDynamicArray::Get<RenderBatchData>(m_Batches, i);
					Log::Assert(!batch.BatchShader.IsNull(), "Cannot render with a null shader.");
					const Shader& shader = AssetManager::GetShader(batch.BatchShader.m_AssetId);
					NShader::Bind(shader);
					NShader::UploadMat4(shader, "uProjection", camera.ProjectionMatrix);
					NShader::UploadMat4(shader, "uView", camera.ViewMatrix);
					NShader::UploadIntArray(shader, "uTextures[0]", 16, m_TexSlots);

					RenderBatch::Render(batch);
					RenderBatch::Clear(batch);
				}

				DebugDraw::DrawTopBatches(camera);
			}
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
					{"Entity", NEntity::GetID(entity)},
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
			NEntity::AddComponent<SpriteRenderer>(entity, spriteRenderer);
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
					{"Entity", NEntity::GetID(entity)},
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
			NEntity::AddComponent<FontRenderer>(entity, fontRenderer);
		}
	}
}