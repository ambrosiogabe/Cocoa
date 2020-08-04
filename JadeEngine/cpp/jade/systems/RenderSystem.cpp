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
	void RenderSystem::AddEntity(const Transform& transform, const SpriteRenderer& spr)
	{
		const Sprite& sprite = spr.m_Sprite;
		bool wasAdded = false;
		for (RenderBatch* batch : m_Batches)
		{
			if (batch->HasRoom())
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
			RenderBatch* newBatch = new RenderBatch(MAX_BATCH_SIZE);
			newBatch->Start();
			newBatch->Add(transform, spr);
			m_Batches.push_back(newBatch);
		}
	}

	void RenderSystem::Render(entt::registry& registry)
	{
		registry.group<SpriteRenderer>(entt::get<Transform>).each([this](auto entity, auto& spr, auto& transform)
			{
				this->AddEntity(transform, spr);
			});

		m_Shader->Bind();
		m_Shader->UploadMat4("uProjection", m_Camera->GetOrthoProjection());
		m_Shader->UploadMat4("uView", m_Camera->GetOrthoView());
		m_Shader->UploadIntArray("uTextures", 16, m_TexSlots);

		for (RenderBatch* batch : m_Batches)
		{
			batch->Render();
			batch->Clear();
		}

		m_Shader->Unbind();
	}

	void RenderSystem::ImGui(entt::registry& registry)
	{
		entt::entity activeEntity = Application::Get()->GetScene()->GetActiveEntity();

		if (registry.has<SpriteRenderer>(activeEntity))
		{
			static bool collapsingHeaderOpen = true;
			ImGui::SetNextTreeNodeOpen(collapsingHeaderOpen);
			if (ImGui::CollapsingHeader("Sprite Renderer"))
			{
				ImGui::BeginCollapsingHeaderGroup();
				SpriteRenderer& spr = registry.get<SpriteRenderer>(activeEntity);
				ImGui::UndoableColorEdit4("Sprite Color: ", spr.m_Color);

				if (spr.m_Sprite.m_Texture)
				{
					ImGui::InputText("##SpriteRendererTexture", (char*)spr.m_Sprite.m_Texture->GetFilepath().Filename(),
						spr.m_Sprite.m_Texture->GetFilepath().FilenameSize(), ImGuiInputTextFlags_ReadOnly);
				}
				else
				{
					ImGui::InputText("##SpriteRendererTexture", "Default Sprite", 14, ImGuiInputTextFlags_ReadOnly);
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

				ImGui::EndCollapsingHeaderGroup();
			}
		}
	}

	void RenderSystem::Serialize(entt::entity entity, const SpriteRenderer& spriteRenderer)
	{
		json& j = Application::Get()->GetScene()->GetSaveDataJson();

		json color = JMath::Serialize("Color", spriteRenderer.m_Color);
		json assetId = { "AssetId", -1 };
		if (spriteRenderer.m_Sprite.m_Texture)
		{
			assetId = { "AssetId", spriteRenderer.m_Sprite.m_Texture->GetResourceId() };
		}

		int size = j["Size"];
		j["Components"][size] = {
			{"SpriteRenderer", {
				{"Entity", entt::to_integral(entity)},
				assetId,
				color
			}}
		};

		j["Size"] = size + 1;
	}

	void RenderSystem::Deserialize(json& j, entt::registry& registry, entt::entity entity)
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
		registry.emplace<SpriteRenderer>(entity, spriteRenderer);
	}
}