#pragma once

#include "LevelEditorSystem.h"
#include "LevelEditorScene.h"
#include "jade/systems/System.h"
#include "jade/renderer/DebugDraw.h"
#include "jade/physics2d/Physics2DSystem.h"
#include "jade/util/JMath.h"

#include <entt/entt.h>
#include <imgui/imgui.h>

namespace Jade {
    void LevelEditorScene::Init() {
        Physics2D::Init(m_Registry);

        auto group = m_Registry.group<SpriteRenderer>(entt::get<Transform>);

        Log::Info("Initializing test scene.");

        m_Systems.emplace_back(std::make_unique<LevelEditorSystem>("LE System"));
        m_Systems.emplace_back(std::make_unique<Physics2DSystem>("Physics 2D System"));

        glm::vec3 cameraPos = glm::vec3(1920.0f/2.0f, 1080.0f/2.0f, 0);
        m_Camera = new Camera(cameraPos);
        m_Systems.emplace_back(std::make_unique<RenderSystem>(m_Camera, "Render System"));

        texture = new Texture("assets/images/decorationsAndBlocks.png", false);
        sprites = new Spritesheet(texture, 16, 16, 81, 0);

        for (int i = 0; i < sprites->Size(); i++)
        {
            entt::entity testEntity = m_Registry.create();
            Sprite* sprite = &sprites->GetSprite(i);

            m_Registry.emplace<Transform>(testEntity, glm::vec3(-256 + (i * 64) + (i * 32), 0, 0), glm::vec3(2, 2, 1), glm::vec3(0, 0, 0), "Textured Block");
            m_Registry.emplace<SpriteRenderer>(testEntity, glm::vec4(1, 1, 1, 1), sprite);
            m_Registry.emplace<BoundingBox>(testEntity, sprite->m_BoundingBox);

            if (i == 0)
            {
                m_ActiveEntity = testEntity;
            }
        }

        entt::entity container = m_Registry.create();
        m_Registry.emplace<Transform>(container, glm::vec3(), glm::vec3(1), glm::vec3(), "Container");

        float startX = 100.0f;
        float startY = 80.0f;
        float width = 20.0f;
        float height = 20.0f;
        float padding = 3.0f;
        entt::entity previous = entt::null;
        for (int i=0; i < 10; i++) {
            for (int j=0; j < 10; j++) {
                float x = startX + i * width + i * padding;
                float y = startY + j * height + j * padding;

                float r = x / (100 * width);
                float g = y / (80 * height);
                float b = 1.0f;

                entt::entity e1 = m_Registry.create();
                m_Registry.emplace<Transform>(e1, glm::vec3(x, y, 0), glm::vec3(0.6f, 0.6f, 1.0f), glm::vec3(0, 0, 0), "Gen Block", container, previous);
                m_Registry.emplace<SpriteRenderer>(e1, glm::vec4(r, g, b, 1));
                previous = e1;
            }
        }
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
        static glm::vec3 color{ 1, 0, 0 };
        m_Registry.group<SpriteRenderer>(entt::get<Transform>).each([this](auto entity, SpriteRenderer& spr, Transform& transform)
            {
                if (spr.m_Sprite->m_PictureFile != nullptr)
                {
                    BoundingBox& box = spr.m_Sprite->m_BoundingBox;
                    DebugDraw::AddBox2D(JMath::Vector2From3(transform.m_Position) + (box.m_Offset * JMath::Vector2From3(transform.m_Scale)), 
                        box.m_Size * JMath::Vector2From3(transform.m_Scale), 0.0f, 1.0f, color);
                }
        });

        for (const auto& system : m_Systems) {
            system->Update(m_Registry, dt);
        }
    }
        
    void LevelEditorScene::ImGui() {
        if (showDemoWindow) {
            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
            ImGui::ShowDemoWindow(&showDemoWindow);
        }

        ImGui::Begin("Inspector");

        if (entt::to_integral(m_ActiveEntity) != entt::to_integral(entt::null))
        {
            for (const auto& system : m_Systems)
            {
                system->ImGui(m_Registry);
                ImGui::Separator();
            }

            static bool componentsOpen = false;
            ImGui::SetCursorPosY(ImGui::GetWindowContentRegionMax().y - 100);
            ImGui::SetNextTreeNodeOpen(componentsOpen);
            if (ImGui::CollapsingHeader("Add Component"))
            {
                componentsOpen = true;
                const char* components[] = { "Sprite Renderer", "Rigidbody", "BoxCollider2D", "AABBCollider2D", "CircleCollider2D" };
                for (int i = 0; i < 3; i++)
                {
                    if (ImGui::Button(components[i]))
                    {
                        switch (i)
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
                        case 3:
                            m_Registry.emplace<AABB>(m_ActiveEntity);
                            break;
                        case 4:
                            m_Registry.emplace<Circle>(m_ActiveEntity);
                            break;
                        }
                        componentsOpen = false;
                        break;
                    }
                }
            }
        }

        ImGui::End();
    }
}