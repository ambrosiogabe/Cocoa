#include "jade/core/MenuBar.h"
#include "jade/components/components.h"
#include "jade/components/Transform.h"
#include "jade/util/Settings.h"
#include "jade/core/Application.h"

#include <entt.h>
#include <imgui.h>

namespace Jade {
    void MenuBar::SettingsWindow() {
        ImGui::Begin("Settings", &m_SettingsOpen);
        if (ImGui::DragInt2("Grid Size: ", m_GridSize)) {
            Settings::GridSizeX = m_GridSize[0];
            Settings::GridSizeY = m_GridSize[1];
        }

        ImGui::Checkbox("Draw Grid: ", &Settings::DrawGrid);
        ImGui::End();   
    }


    void MenuBar::ImGui() {
        if (m_SettingsOpen) {
            SettingsWindow();
        }

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Jade")) {
                if (ImGui::Button("Settings")) {
                    m_SettingsOpen = true;
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Game Objects")) {
                if (ImGui::Button("Add Sprite Object")) {
                    entt::registry& registry = Application::Get()->GetScene()->GetRegistry();
                    entt::entity entity = registry.create();
                    registry.emplace<Transform>(entity, glm::vec3(), glm::vec3(1, 1, 1), glm::vec3());
                    registry.emplace<SpriteRenderer>(entity, glm::vec4(1, 1, 1, 1));
                    registry.emplace<AABB>(entity, glm::vec2(32, 32), glm::vec2(16, 16), glm::vec2());
                }

                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    }
}