#include "core/MenuBar.h"
#include "core/JWindow.h"
#include "components/components.h"
#include "util/Constants.h"

#include <entt/entt.h>
#include <imgui/imgui.h>

namespace Jade {
    void MenuBar::SettingsWindow() {
        ImGui::Begin("Settings", &m_SettingsOpen);
        if (ImGui::DragInt2("Grid Size: ", m_GridSize)) {
            Constants::GridSizeX = m_GridSize[0];
            Constants::GridSizeY = m_GridSize[1];
        }
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
                    entt::registry& registry = JWindow::Get()->GetScene()->GetRegistry();
                    entt::entity entity = registry.create();
                    registry.emplace<Transform>(entity, glm::vec3(), glm::vec3(32, 32, 1), glm::vec3());
                    registry.emplace<SpriteRenderer>(entity, glm::vec4(1, 1, 1, 1));
                }

                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    }
}