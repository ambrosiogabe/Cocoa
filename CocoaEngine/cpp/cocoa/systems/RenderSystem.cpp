#include "externalLibs.h"

#include "cocoa/util/Log.h"
#include "cocoa/systems/RenderSystem.h"
#include "cocoa/core/Application.h"
#include "cocoa/components/components.h"
#include "cocoa/commands/ICommand.h"
#include "cocoa/util/CMath.h"

#include <nlohmann/json.hpp>

namespace Cocoa
{
	std::shared_ptr<Shader> RenderSystem::s_Shader = nullptr;

	void RenderSystem::AddEntity(const Transform& transform, const SpriteRenderer& spr)
	{
		const Sprite& sprite = spr.m_Sprite;
		bool wasAdded = false;
		for (auto& batch : m_Batches)
		{
			if (batch->HasRoom() && spr.m_ZIndex == batch->ZIndex())
			{
				Handle<Texture> tex = sprite.m_Texture;
				if (!tex || batch->HasTexture(tex) || batch->HasTextureRoom())
				{
					batch->Add(transform, spr);
					wasAdded = true;
					break;
				}
			}
		}

		if (!wasAdded)
		{
			std::shared_ptr<RenderBatch> newBatch = std::make_shared<RenderBatch>(MAX_BATCH_SIZE, spr.m_ZIndex);
			newBatch->Start();
			newBatch->Add(transform, spr);
			m_Batches.emplace_back(newBatch);
			std::sort(m_Batches.begin(), m_Batches.end(), RenderBatch::Compare);
		}
	}

	void RenderSystem::AddEntity(const Transform& transform, const FontRenderer& fontRenderer)
	{
		const Font& font = AssetManager::GetFont(fontRenderer.m_Font.m_AssetId);
		bool wasAdded = false;
		for (auto& batch : m_Batches)
		{
			if (batch->HasRoom(fontRenderer) && fontRenderer.m_ZIndex == batch->ZIndex())
			{
				Handle<Texture> tex = font.m_FontTexture;
				if (!tex || batch->HasTexture(tex) || batch->HasTextureRoom())
				{
					batch->Add(transform, fontRenderer);
					wasAdded = true;
					break;
				}
			}
		}

		if (!wasAdded)
		{
			std::shared_ptr<RenderBatch> newBatch = std::make_shared<RenderBatch>(MAX_BATCH_SIZE, fontRenderer.m_ZIndex);
			newBatch->Start();
			newBatch->Add(transform, fontRenderer);
			m_Batches.emplace_back(newBatch);
			std::sort(m_Batches.begin(), m_Batches.end(), RenderBatch::Compare);
		}
	}

	void RenderSystem::Render()
	{
		m_Scene->GetRegistry().group<SpriteRenderer>(entt::get<Transform>).each([this](auto entity, auto& spr, auto& transform)
		{
			this->AddEntity(transform, spr);
		});

		m_Scene->GetRegistry().group<FontRenderer>(entt::get<Transform>).each([this](auto entity, auto& fontRenderer, auto& transform)
		{
			this->AddEntity(transform, fontRenderer);
		});

		Log::Assert((s_Shader != nullptr), "Must bind shader before render call");

		s_Shader->Bind();
		s_Shader->UploadMat4("uProjection", m_Camera->GetOrthoProjection());
		s_Shader->UploadMat4("uView", m_Camera->GetOrthoView());
		s_Shader->UploadIntArray("uTextures", 16, m_TexSlots);
		//s_Shader->UploadFloat("uActiveEntityID", (float)(m_Scene->GetActiveEntity().GetID() + 1));

		for (auto& batch : m_Batches)
		{
			batch->Render();
			batch->Clear();
		}

		s_Shader->Unbind();
	}

	void RenderSystem::Serialize(json& j, Entity entity, const SpriteRenderer& spriteRenderer)
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

	void RenderSystem::DeserializeSpriteRenderer(json& j, Entity entity)
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

	void RenderSystem::Serialize(json& j, Entity entity, const FontRenderer& fontRenderer)
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

	void RenderSystem::DeserializeFontRenderer(json& j, Entity entity)
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