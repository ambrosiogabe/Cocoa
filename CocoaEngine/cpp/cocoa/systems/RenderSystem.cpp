#include "externalLibs.h"

#include "cocoa/systems/RenderSystem.h"
#include "cocoa/core/AssetManager.h"
#include "cocoa/util/CMath.h"
#include "cocoa/renderer/DebugDraw.h"
#include "cocoa/renderer/CameraStruct.h"
#include "cocoa/renderer/RenderBatch.h"
#include "cocoa/util/Settings.h"

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

		void init()
		{
			m_Batches = List<RenderBatchData>(1);

			PathBuilder spriteShaderPath = Settings::General::engineAssetsPath;
			spriteShaderPath.join("shaders/SpriteRenderer.glsl");
			m_SpriteShader = AssetManager::loadShaderFromFile(spriteShaderPath.createPath(), true);
			
			PathBuilder fontShaderPath = Settings::General::engineAssetsPath;
			fontShaderPath.join("shaders/FontRenderer.glsl");
			m_FontShader = AssetManager::loadShaderFromFile(fontShaderPath.createPath(), true);
			
			PathBuilder pickingShaderPath = Settings::General::engineAssetsPath;
			pickingShaderPath.join("shaders/Picking.glsl");
			AssetManager::loadShaderFromFile(pickingShaderPath.createPath(), true);
		}

		void destroy()
		{
			for (int i = 0; i < m_Batches.size(); i++)
			{
				RenderBatch::free(m_Batches[i]);
			}
		}

		void addEntity(const TransformData& transform, const SpriteRenderer& spr)
		{
			const Sprite& sprite = spr.sprite;
			bool wasAdded = false;
			for (int i = 0; i < m_Batches.size(); i++)
			{
				RenderBatchData& batch = m_Batches[i];
				if (RenderBatch::hasRoom(batch) && spr.zIndex == batch.zIndex && batch.batchShader == m_SpriteShader)
				{
					Handle<Texture> tex = sprite.texture;
					if (!tex || RenderBatch::hasTexture(batch, tex) || RenderBatch::hasTextureRoom(batch))
					{
						RenderBatch::add(batch, transform, spr);
						wasAdded = true;
						break;
					}
				}
			}

			if (!wasAdded)
			{
				RenderBatchData newBatch = RenderBatch::createRenderBatch(MAX_BATCH_SIZE, spr.zIndex, m_SpriteShader);
				RenderBatch::start(newBatch);
				RenderBatch::add(newBatch, transform, spr);
				m_Batches.push(newBatch);
				std::sort(m_Batches.begin(), m_Batches.end(), RenderBatch::compare);
			}
		}

		void addEntity(const TransformData& transform, const FontRenderer& fontRenderer)
		{
			const Font& font = AssetManager::getFont(fontRenderer.font.assetId);
			bool wasAdded = false;
			for (int i = 0; i < m_Batches.size(); i++)
			{
				RenderBatchData& batch = m_Batches[i];
				if (RenderBatch::hasRoom(batch, fontRenderer) && fontRenderer.zIndex == batch.zIndex && batch.batchShader == m_FontShader)
				{
					Handle<Texture> tex = font.fontTexture;
					if (!tex || RenderBatch::hasTexture(batch, tex) || RenderBatch::hasTextureRoom(batch))
					{
						RenderBatch::add(batch, transform, fontRenderer);
						wasAdded = true;
						break;
					}
				}
			}

			if (!wasAdded)
			{
				RenderBatchData newBatch = RenderBatch::createRenderBatch(MAX_BATCH_SIZE, fontRenderer.zIndex, m_FontShader);
				RenderBatch::start(newBatch);
				RenderBatch::add(newBatch, transform, fontRenderer);
				m_Batches.push(newBatch);
				std::sort(m_Batches.begin(), m_Batches.end(), RenderBatch::compare);
			}
		}

		void render(const SceneData& scene)
		{
			scene.registry.view<const SpriteRenderer, const TransformData>().each([](auto entity, const auto& spriteRenderer, const auto& transform)
				{
					addEntity(transform, spriteRenderer);
				});

			scene.registry.view<const FontRenderer, const TransformData>().each([](auto entity, const auto& fontRenderer, const auto& transform)
				{
					addEntity(transform, fontRenderer);
				});

			DebugDraw::addDebugObjectsToBatches();

			auto view = scene.registry.view<const Camera>();
			for (auto& rawEntity : view)
			{
				const Camera& camera = scene.registry.get<Camera>(rawEntity);
				NFramebuffer::bind(camera.framebuffer);

				glEnable(GL_BLEND); // TODO: This should be encapsulated within the camera somehow
				glViewport(0, 0, 3840, 2160); // TODO: This should be encapsulated within the camera somehow
				NCamera::clearColorRgb(camera, 0, camera.clearColor);
				//RenderSystem::UploadUniform1ui("uActiveEntityID", InspectorWindow::GetActiveEntity().GetID() + 1);

				DebugDraw::drawBottomBatches(camera);

				for (int i = 0; i < m_Batches.size(); i++)
				{
					RenderBatchData& batch = m_Batches[i];
					Logger::Assert(!batch.batchShader.isNull(), "Cannot render with a null shader.");
					const Shader& shader = AssetManager::getShader(batch.batchShader.assetId);
					NShader::bind(shader);
					NShader::uploadMat4(shader, "uProjection", camera.projectionMatrix);
					NShader::uploadMat4(shader, "uView", camera.viewMatrix);
					NShader::uploadIntArray(shader, "uTextures[0]", 16, m_TexSlots);

					RenderBatch::render(batch);
				}

				DebugDraw::drawTopBatches(camera);
			}

			for (int i = 0; i < m_Batches.size(); i++)
			{
				RenderBatch::clear(m_Batches[i]);
				DebugDraw::clearAllBatches();
			}
		}

		void serialize(json& j, Entity entity, const SpriteRenderer& spriteRenderer)
		{
			json color = CMath::serialize("Color", spriteRenderer.color);
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

		void deserializeSpriteRenderer(const json& j, Entity entity)
		{
			SpriteRenderer spriteRenderer;
			spriteRenderer.color = CMath::deserializeVec4(j["SpriteRenderer"]["Color"]);
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

		void serialize(json& j, Entity entity, const FontRenderer& fontRenderer)
		{
			json color = CMath::serialize("Color", fontRenderer.color);
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

		void deserializeFontRenderer(const json& j, Entity entity)
		{
			FontRenderer fontRenderer;
			fontRenderer.color = CMath::deserializeVec4(j["FontRenderer"]["Color"]);
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