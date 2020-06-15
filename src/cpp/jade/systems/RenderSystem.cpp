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

void RenderSystem::Render() {
    //auto view = m_Registry.view<Transform, SpriteRenderer>();

    m_Registry.group<SpriteRenderer>(entt::get<Transform>).each([this](auto entity, auto& spr, auto& transform) {
        this->AddEntity(transform, spr);
    });


    // m_Registry.view<Transform, SpriteRenderer>().each([this](auto entity, auto& transform, auto& spr) {
    //     this->AddEntity(transform, spr);
    // });
    // for (auto e : view) {
    //     AddEntity(e);
    // }

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

void RenderSystem::ImGui() {
    auto view = m_Registry.view<SpriteRenderer>();

    for (entt::entity entity : view) {
        if (JWindow::GetScene()->GetActiveEntity() == &entity) {
            SpriteRenderer spr = view.get<SpriteRenderer>(entity);
            if (ImGui::ColorPicker4("Sprite Color: ", glm::value_ptr(spr.m_Color))) {
                Log::Info("Color: %2.3f %2.3f %2.3f %2.3f", spr.m_Color.r, spr.m_Color.g, spr.m_Color.b, spr.m_Color.a);
            }
        }
    }
}