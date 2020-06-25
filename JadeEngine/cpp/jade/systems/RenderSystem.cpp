#include "jade/systems/RenderSystem.h"
#include "jade/util/Log.h"
#include "jade/core/Application.h"
#include "jade/components/components.h"
#include "jade/commands/CommandHistory.h"
#include "jade/commands/ChangeSpriteColorCommand.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace Jade {
    const int RenderSystem::MAX_BATCH_SIZE = 1000;

    void RenderSystem::AddEntity(const Transform& transform, const SpriteRenderer& spr) {    
        Sprite* sprite = spr.m_Sprite;
        bool wasAdded = false;
        for (RenderBatch* batch : m_Batches) {
            if (batch->HasRoom()) {
                Texture* tex = sprite->m_Texture;
                if (tex == nullptr || (batch->HasTexture(tex) || batch->HasTextureRoom())) {
                    batch->Add(transform, spr);
                    wasAdded = true;
                    break;
                }
            }
        }

        if (!wasAdded) {
            RenderBatch* newBatch = new RenderBatch(this);
            newBatch->Start();
            newBatch->Add(transform, spr);
            m_Batches.push_back(newBatch);
        }
    }

    void RenderSystem::Render(entt::registry& registry) {
        registry.group<SpriteRenderer>(entt::get<Transform>).each([this](auto entity, auto& spr, auto& transform) {
            this->AddEntity(transform, spr);
        });

        m_Shader->Bind();
        m_Shader->UploadMat4("uProjection", m_Camera->GetOrthoProjection());
        m_Shader->UploadMat4("uView", m_Camera->GetOrthoView());
        m_Shader->UploadIntArray("uTextures", 16, m_TexSlots);

        for (RenderBatch* batch : m_Batches) {
            batch->Render();
            batch->Clear();
        }

        m_Shader->Unbind();
    }

    void RenderSystem::ImGui(entt::registry& registry) {
        entt::entity activeEntity = Application::Get()->GetScene()->GetActiveEntity();

        if (registry.has<SpriteRenderer>(activeEntity)) {
            if (ImGui::CollapsingHeader("Sprite Renderer")) {
                SpriteRenderer& spr = registry.get<SpriteRenderer>(activeEntity);
                glm::vec4 color = glm::vec4(spr.m_Color);
                if (ImGui::ColorPicker4("Sprite Color: ", glm::value_ptr(color))) {
                    CommandHistory::AddCommand(new ChangeSpriteColorCommand(spr, color));
                }

                if (ImGui::IsItemDeactivatedAfterEdit()) {
                    CommandHistory::SetNoMergeMostRecent();
                }
            }
        }
    }
}