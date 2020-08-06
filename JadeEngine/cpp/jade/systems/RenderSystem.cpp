#include "externalLibs.h"

#include "jade/util/Log.h"
#include "jade/systems/RenderSystem.h"
#include "jade/core/Application.h"
#include "jade/components/components.h"
#include "jade/commands/ICommand.h"
#include "jade/util/JMath.h"
#include "jade/core/ImGuiExtended.h"

#include <nlohmann/json.hpp>

namespace Jade
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
				std::shared_ptr<Texture> tex = sprite.m_Texture;
				if (tex == nullptr || (batch->HasTexture(tex->GetResourceId()) || batch->HasTextureRoom()))
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

	void RenderSystem::Render()
	{
		m_Scene->GetRegistry().group<SpriteRenderer>(entt::get<Transform>).each([this](auto entity, auto& spr, auto& transform)
		{
			this->AddEntity(transform, spr);
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

	void RenderSystem::ImGui()
	{
		Entity activeEntity = Entity::Null;// m_Scene->GetActiveEntity();

		if (activeEntity.HasComponent<SpriteRenderer>())
		{
			static bool collapsingHeaderOpen = true;
			ImGui::SetNextTreeNodeOpen(collapsingHeaderOpen);
			if (ImGui::CollapsingHeader("Sprite Renderer"))
			{
				JImGui::BeginCollapsingHeaderGroup();
				SpriteRenderer& spr = activeEntity.GetComponent<SpriteRenderer>();
				JImGui::UndoableDragInt("Z-Index: ", spr.m_ZIndex);
				JImGui::UndoableColorEdit4("Sprite Color: ", spr.m_Color);

				if (spr.m_Sprite.m_Texture)
				{
					JImGui::InputText("##SpriteRendererTexture", (char*)spr.m_Sprite.m_Texture->GetFilepath().Filename(),
						spr.m_Sprite.m_Texture->GetFilepath().FilenameSize(), ImGuiInputTextFlags_ReadOnly);
				}
				else
				{
					JImGui::InputText("##SpriteRendererTexture", "Default Sprite", 14, ImGuiInputTextFlags_ReadOnly);
				}
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_HANDLE_ID"))
					{
						IM_ASSERT(payload->DataSize == sizeof(int));
						int textureResourceId = *(const int*)payload->Data;
						spr.m_Sprite.m_Texture = std::static_pointer_cast<Texture>(AssetManager::GetAsset(textureResourceId));
					}
					ImGui::EndDragDropTarget();
				}

				JImGui::EndCollapsingHeaderGroup();
			}
		}
	}

	void RenderSystem::Serialize(json& j, Entity entity, const SpriteRenderer& spriteRenderer)
	{
		json color = JMath::Serialize("Color", spriteRenderer.m_Color);
		json assetId = { "AssetId", -1 };
		json zIndex = { "ZIndex", spriteRenderer.m_ZIndex };
		if (spriteRenderer.m_Sprite.m_Texture)
		{
			assetId = { "AssetId", spriteRenderer.m_Sprite.m_Texture->GetResourceId() };
		}

		int size = j["Size"];
		j["Components"][size] = {
			{"SpriteRenderer", {
				{"Entity", entity.GetID()},
				assetId,
				zIndex,
				color
			}}
		};

		j["Size"] = size + 1;
	}

	void RenderSystem::Deserialize(json& j, Entity entity)
	{
		SpriteRenderer spriteRenderer;
		spriteRenderer.m_Color = JMath::DeserializeVec4(j["SpriteRenderer"]["Color"]);
		if (!j["SpriteRenderer"]["AssetId"].is_null())
		{
			if (j["SpriteRenderer"]["AssetId"] != -1)
			{
				spriteRenderer.m_Sprite.m_Texture = std::static_pointer_cast<Texture>(AssetManager::GetAsset((uint32)j["SpriteRenderer"]["AssetId"]));
			}
		}

		if (!j["SpriteRenderer"]["ZIndex"].is_null())
		{
			spriteRenderer.m_ZIndex = j["SpriteRenderer"]["ZIndex"];
		}
		entity.AddComponent<SpriteRenderer>(spriteRenderer);
	}
}