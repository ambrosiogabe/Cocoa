#include "externalLibs.h"

#include "cocoa/systems/RenderSystem.h"
#include "cocoa/core/Application.h"
#include "cocoa/core/AssetManager.h"
#include "cocoa/commands/ICommand.h"
#include "cocoa/util/CMath.h"
#include "cocoa/renderer/DebugDraw.h"
#include "cocoa/renderer/CameraStruct.h"

#include <nlohmann/json.hpp>

namespace Cocoa
{
	namespace RenderSystem
	{
		// Internal Variables
		static Handle<Shader> m_SpriteShader = NHandle::createHandle<Shader>();
		static Handle<Shader> m_FontShader = NHandle::createHandle<Shader>();

		static int m_TexSlots[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
		static const int MAX_BATCH_SIZE = 1000;

		static List<RenderBatchData> m_Batches;

		void Init()
		{
			m_Batches = List<RenderBatchData>(1);

			CPath spriteShaderPath = Settings::General::s_EngineAssetsPath;
			spriteShaderPath.join(CPath::create("shaders/SpriteRenderer.glsl"));
			m_SpriteShader = AssetManager::loadShaderFromFile(spriteShaderPath, true);
			CPath fontShaderPath = Settings::General::s_EngineAssetsPath;
			fontShaderPath.join(CPath::create("shaders/FontRenderer.glsl"));
			m_FontShader = AssetManager::loadShaderFromFile(fontShaderPath, true);
			CPath pickingShaderPath = Settings::General::s_EngineAssetsPath;
			pickingShaderPath.join(CPath::create("shaders/Picking.glsl"));
			AssetManager::loadShaderFromFile(pickingShaderPath, true);
		}

		void Destroy()
		{
			for (int i = 0; i < m_Batches.size(); i++)
			{
				RenderBatch::Free(m_Batches[i]);
			}
		}

		void AddEntity(const TransformData& transform, const SpriteRenderer& spr)
		{
			const Sprite& sprite = spr.sprite;
			bool wasAdded = false;
			for (int i = 0; i < m_Batches.size(); i++)
			{
				RenderBatchData& batch = m_Batches[i];
				if (RenderBatch::HasRoom(batch) && spr.zIndex == batch.ZIndex && batch.BatchShader == m_SpriteShader)
				{
					Handle<Texture> tex = sprite.texture;
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
				RenderBatchData newBatch = RenderBatch::CreateRenderBatch(MAX_BATCH_SIZE, spr.zIndex, m_SpriteShader);
				RenderBatch::Start(newBatch);
				RenderBatch::Add(newBatch, transform, spr);
				m_Batches.push(newBatch);
				std::sort(m_Batches.begin(), m_Batches.end(), RenderBatch::Compare);
			}
		}

		void AddEntity(const TransformData& transform, const FontRenderer& fontRenderer)
		{
			const Font& font = AssetManager::getFont(fontRenderer.font.assetId);
			bool wasAdded = false;
			for (int i = 0; i < m_Batches.size(); i++)
			{
				RenderBatchData& batch = m_Batches[i];
				if (RenderBatch::HasRoom(batch, fontRenderer) && fontRenderer.zIndex == batch.ZIndex && batch.BatchShader == m_FontShader)
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
				RenderBatchData newBatch = RenderBatch::CreateRenderBatch(MAX_BATCH_SIZE, fontRenderer.zIndex, m_FontShader);
				RenderBatch::Start(newBatch);
				RenderBatch::Add(newBatch, transform, fontRenderer);
				m_Batches.push(newBatch);
				std::sort(m_Batches.begin(), m_Batches.end(), RenderBatch::Compare);
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

			DebugDraw::AddDebugObjectsToBatches();

			auto view = scene.Registry.view<const Camera>();
			for (auto& rawEntity : view)
			{
				const Camera& camera = scene.Registry.get<Camera>(rawEntity);
				NFramebuffer::Bind(camera.Framebuffer);

				glEnable(GL_BLEND); // TODO: This should be encapsulated within the camera somehow
				glViewport(0, 0, 3840, 2160); // TODO: This should be encapsulated within the camera somehow
				NCamera::ClearColorRgb(camera, 0, camera.ClearColor);
				//RenderSystem::UploadUniform1ui("uActiveEntityID", InspectorWindow::GetActiveEntity().GetID() + 1);

				DebugDraw::DrawBottomBatches(camera);

				for (int i = 0; i < m_Batches.size(); i++)
				{
					RenderBatchData& batch = m_Batches[i];
					Logger::Assert(!batch.BatchShader.isNull(), "Cannot render with a null shader.");
					const Shader& shader = AssetManager::getShader(batch.BatchShader.assetId);
					NShader::Bind(shader);
					NShader::UploadMat4(shader, "uProjection", camera.ProjectionMatrix);
					NShader::UploadMat4(shader, "uView", camera.ViewMatrix);
					NShader::UploadIntArray(shader, "uTextures[0]", 16, m_TexSlots);

					RenderBatch::Render(batch);
				}

				DebugDraw::DrawTopBatches(camera);
			}

			for (int i = 0; i < m_Batches.size(); i++)
			{
				RenderBatch::Clear(m_Batches[i]);
				DebugDraw::ClearAllBatches();
			}
		}

		void Serialize(json& j, Entity entity, const SpriteRenderer& spriteRenderer)
		{
			json color = CMath::Serialize("Color", spriteRenderer.color);
			json assetId = { "AssetId", (uint32)std::numeric_limits<uint32>::max() };
			json zIndex = { "ZIndex", spriteRenderer.zIndex };
			if (spriteRenderer.sprite.texture)
			{
				assetId = { "AssetId", spriteRenderer.sprite.texture.assetId };
			}

			int size = j["Components"].size();
			j["Components"][size] = {
				{"SpriteRenderer", {
					{"Entity", NEntity::getId(entity)},
					assetId,
					zIndex,
					color
				}}
			};
		}

		void DeserializeSpriteRenderer(const json& j, Entity entity)
		{
			SpriteRenderer spriteRenderer;
			spriteRenderer.color = CMath::DeserializeVec4(j["SpriteRenderer"]["Color"]);
			if (j["SpriteRenderer"].contains("AssetId"))
			{
				if (j["SpriteRenderer"]["AssetId"] != std::numeric_limits<uint32>::max())
				{
					spriteRenderer.sprite.texture = NHandle::createHandle<Texture>(j["SpriteRenderer"]["AssetId"]);
				}
			}

			if (j["SpriteRenderer"].contains("ZIndex"))
			{
				spriteRenderer.zIndex = j["SpriteRenderer"]["ZIndex"];
			}
			NEntity::addComponent<SpriteRenderer>(entity, spriteRenderer);
		}

		void Serialize(json& j, Entity entity, const FontRenderer& fontRenderer)
		{
			json color = CMath::Serialize("Color", fontRenderer.color);
			json assetId = { "AssetId", (uint32)std::numeric_limits<uint32>::max() };
			json zIndex = { "ZIndex", fontRenderer.zIndex };
			json text = { "Text", fontRenderer.text };
			json fontSize = { "FontSize", fontRenderer.fontSize };
			if (fontRenderer.font)
			{
				assetId = { "AssetId", fontRenderer.font.assetId };
			}

			int size = j["Components"].size();
			j["Components"][size] = {
				{"FontRenderer", {
					{"Entity", NEntity::getId(entity)},
					assetId,
					zIndex,
					color,
					text,
					fontSize
				}}
			};
		}

		void DeserializeFontRenderer(const json& j, Entity entity)
		{
			FontRenderer fontRenderer;
			fontRenderer.color = CMath::DeserializeVec4(j["FontRenderer"]["Color"]);
			if (j["FontRenderer"].contains("AssetId"))
			{
				if (j["FontRenderer"]["AssetId"] != std::numeric_limits<uint32>::max())
				{
					fontRenderer.font = NHandle::createHandle<Font>(j["FontRenderer"]["AssetId"]);
				}
			}

			if (j["FontRenderer"].contains("ZIndex"))
			{
				fontRenderer.zIndex = j["FontRenderer"]["ZIndex"];
			}

			if (j["FontRenderer"].contains("Text"))
			{
				fontRenderer.text = j["FontRenderer"]["Text"];
			}

			if (j["FontRenderer"].contains("FontSize"))
			{
				fontRenderer.fontSize = j["FontRenderer"]["FontSize"];
			}
			NEntity::addComponent<FontRenderer>(entity, fontRenderer);
		}
	}
}