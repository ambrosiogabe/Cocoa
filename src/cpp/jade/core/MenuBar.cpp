#include "jade/core/MenuBar.h"

#include "jade/platform/JWindow.h"
#include "jade/components/components.h"

#include <entt/entt.h>
#include <imgui/imgui.h>

void MenuBar::ImGui() {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Game Objects")) {
            if (ImGui::Button("Add Sprite Object")) {
                entt::registry& registry = JWindow::GetScene()->GetRegistry();
                entt::entity entity = registry.create();
                registry.assign<Transform>(entity, glm::vec3(), glm::vec3(32, 32, 1), glm::vec3());
                registry.assign<SpriteRenderer>(entity, glm::vec4(1, 1, 1, 1));
            }

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}