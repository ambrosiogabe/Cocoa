#pragma once

#include "LevelEditorScene.h"
#include "LevelEditorSystem.h"
#include "Gizmos.h"
#include "jade/systems/System.h"
#include "jade/renderer/DebugDraw.h"
#include "jade/physics2d/Physics2DSystem.h"
#include "jade/util/JMath.h"
#include "jade/core/ImGuiExtended.h"
#include "jade/util/Settings.h"
#include "jade/file/JPath.h"
#include "jade/core/AssetManager.h"
#include "FontAwesome.h"
#include "jade/core/Audio.h"

#include <entt/entt.hpp>
#include <imgui/imgui.h>
#include <array>

namespace Jade {
    void LevelEditorScene::Init() {
        Physics2D::Init(m_Registry);

        m_Systems.emplace_back(std::make_unique<LevelEditorSystem>("LevelEditor System"));
        m_Systems.emplace_back(std::make_unique<Physics2DSystem>("Physics2D System"));
        m_Systems.emplace_back(std::unique_ptr<GizmoSystem>(new GizmoSystem()));

        glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0);
        m_Camera = new Camera(cameraPos);
        m_Systems.emplace_back(std::make_unique<RenderSystem>(m_Camera, "Render System"));

        texture = std::static_pointer_cast<Texture>(AssetManager::LoadTextureFromFile("assets/images/decorationsAndBlocks.png"));
        sprites = new Spritesheet(texture, 16, 16, 81, 0);
    }

    void LevelEditorScene::Start() {
        shader = new Shader("assets/shaders/SpriteRenderer.glsl");

        for (const auto& system : m_Systems) {
            system->Start(m_Registry);
        }
    }

    void LevelEditorScene::Render() {
        for (const auto& system : m_Systems) {
            system->Render(m_Registry);
        }
    }

    void LevelEditorScene::Update(float dt) {
        if (m_IsRunning)
        {
            Physics2D::Get()->Update(dt);
        }

        for (const auto& system : m_Systems) {
            system->Update(m_Registry, dt);
        }
    }
        
    void LevelEditorScene::ImGui() {
        if (m_ShowDemoWindow) {
            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
            ImGui::ShowDemoWindow(&m_ShowDemoWindow);
        }

        ImGuiSceneHeirarchy();

        ImGui::Begin(ICON_FA_CUBE " Inspector");

        if (entt::to_integral(m_ActiveEntity) != entt::to_integral(entt::null))
        {
            for (const auto& system : m_Systems)
            {
                system->ImGui(m_Registry);
            }

            int itemPressed = 0;
            std::array<const char*, 4> components = { "Sprite Renderer", "Rigidbody", "BoxCollider2D", "CircleCollider2D" };
            if (ImGui::JButtonDropdown(ICON_FA_PLUS " Add Component", components.data(), (int)components.size(), itemPressed))
            {
                switch (itemPressed)
                {
                case 0:
                    m_Registry.emplace<SpriteRenderer>(m_ActiveEntity);
                    break;
                case 1:
                    m_Registry.emplace<Rigidbody2D>(m_ActiveEntity);
                    break;
                case 2:
                    m_Registry.emplace<Box2D>(m_ActiveEntity);
                    break;
                case 4:
                    m_Registry.emplace<Circle>(m_ActiveEntity);
                    break;
                }
            }
        }

        ImGui::End();
    }


    void LevelEditorScene::ImGuiSceneHeirarchy()
    {
        ImGui::Begin(ICON_FA_PROJECT_DIAGRAM " Scene");
        int index = 0;
        auto view = m_Registry.view<Transform>();

        for (auto entity : view)
        {
            Transform& transform = view.get<Transform>(entity);
            if (entt::to_integral(transform.m_Parent) == entt::to_integral(entt::null))
            {
                DoTreeNode(index, transform);
            }
        }
        ImGui::End();
    }

    void LevelEditorScene::DoTreeNode(int& index, const Transform& transform)
    {
        static bool isDark = true;
        std::string str = transform.m_Name;
        std::string res = str + "##" + std::to_string(index);
        index++;

        entt::entity next = transform.m_Next;
        while (entt::to_integral(next) != entt::to_integral(entt::null))
        {
            const Transform& transform2 = m_Registry.get<Transform>(next);
            std::string str2 = transform2.m_Name;
            std::string res2 = str2 + "##" + std::to_string(index);
            index++;

            if (ImGui::TreeNodeEx(res2.c_str(), ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth))
            {
                if (entt::to_integral(transform2.m_First) != entt::to_integral(entt::null))
                {
                    const Transform& childTrans = m_Registry.get<Transform>(transform2.m_First);
                    DoTreeNode(index, childTrans);
                }
                ImGui::TreePop();
            }
            next = transform2.m_Next;
        }

        if (ImGui::TreeNodeEx(res.c_str(), ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth))
        {
            if (entt::to_integral(transform.m_First) != entt::to_integral(entt::null))
            {
                const Transform& firstTransform = m_Registry.get<Transform>(transform.m_First);
                DoTreeNode(index, firstTransform);
            }
            ImGui::TreePop();
        }
    }
}