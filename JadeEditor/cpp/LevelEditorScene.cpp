#pragma once

#include "LevelEditorSystem.h"
#include "LevelEditorScene.h"
#include "jade/systems/System.h"

#include <entt/entt.h>
#include <imgui/imgui.h>

namespace Jade {
    void LevelEditorScene::Init() {
        auto group = m_Registry.group<SpriteRenderer>(entt::get<Transform>);

        Log::Info("Initializing test scene.");

        m_Systems.emplace_back(std::make_unique<LevelEditorSystem>("LE System"));

        glm::vec3 cameraPos = glm::vec3(1920.0f/2.0f, 1080.0f/2.0f, 0);
        m_Camera = new Camera(cameraPos);
        m_Systems.emplace_back(std::make_unique<RenderSystem>(m_Camera, "Render System"));

        texture = new Texture("assets/images/decorationsAndBlocks.png");
        sprites = new Spritesheet(texture, 16, 16, 81, 0);

        entt::entity testEntity = m_Registry.create();
        m_Registry.emplace<Transform>(testEntity, glm::vec3(-100, 0, 0), glm::vec3(64, 64, 1), glm::vec3(0, 0, 0), "Textured Block");
        m_Registry.emplace<SpriteRenderer>(testEntity, glm::vec4(1, 1, 1, 1), sprites->GetSprite(80));
        m_ActiveEntity = testEntity;

        entt::entity container = m_Registry.create();
        m_Registry.emplace<Transform>(container, glm::vec3(), glm::vec3(1), glm::vec3(), "Container");

        float startX = 100.0f;
        float startY = 80.0f;
        float width = 20.0f;
        float height = 20.0f;
        float padding = 3.0f;
        entt::entity previous = entt::null;
        for (int i=0; i < 100; i++) {
            for (int j=0; j < 100; j++) {
                float x = startX + i * width + i * padding;
                float y = startY + j * height + j * padding;

                float r = x / (100 * width);
                float g = y / (80 * height);
                float b = 1.0f;

                entt::entity e1 = m_Registry.create();
                m_Registry.emplace<Transform>(e1, glm::vec3(x, y, 0), glm::vec3(width, height, 1.0f), glm::vec3(0, 0, 0), "Gen Block", container, previous);
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

        for (const auto& system : m_Systems) {
            system->ImGui(m_Registry);
            ImGui::Separator();
        }

        static bool componentsOpen = false;
        ImGui::SetCursorPosY(ImGui::GetWindowContentRegionMax().y - 100);
        ImGui::SetNextTreeNodeOpen(componentsOpen);
        if (ImGui::CollapsingHeader("Add Component")) {
            componentsOpen = true;
            const char* components[] = {"Sprite Renderer", "Rigidbody", "Collider"};
            for (int i=0; i < 3; i++) {
                if (ImGui::Button(components[i])) {
                    Log::Info("%s", components[i]);
                    componentsOpen = false;
                    break;
                }
            }
        }

        ImGui::End();
    }
}