#include "jade/systems/RenderSystem.h"
#include "jade/util/Log.h"
#include "jade/platform/JWindow.h"
#include "jade/components/components.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

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
    registry.group<SpriteRenderer>(entt::get<Transform>).each([this](auto& entity, auto& spr, auto& transform) {
        if (entt::to_integral(JWindow::GetScene()->GetActiveEntity()) == entt::to_integral(entity)) {
            ImGui::LabelText("Sprite Renderer", "");
            ImGui::ColorPicker4("Sprite Color: ", glm::value_ptr(spr.m_Color));
            ImGui::Separator();
        }
    });
}