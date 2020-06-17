#include "jade/systems/LevelEditorSystem.h"
#include "jade/platform/JWindow.h"
#include "jade/events/Input.h"

#include <string>
#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

void LevelEditorSystem::Start(entt::registry& registry) {
    auto view = registry.view<Transform>();
    for (auto entity : view) {
        Transform& transform = view.get<Transform>(entity);
        if (entt::to_integral(transform.m_Previous) != entt::to_integral(entt::null)) {
            Transform& prevTransform = view.get<Transform>(transform.m_Previous);
            prevTransform.m_Next = entity;

            if (entt::to_integral(transform.m_Parent) != entt::to_integral(entt::null)) {
                Transform& parentTransform = view.get<Transform>(transform.m_Parent);
                if (entt::to_integral(transform.m_First) == entt::to_integral(entt::null)) {
                    parentTransform.m_First = entity;
                }
            }
        }
    }
}

void LevelEditorSystem::Update(entt::registry& registry, float dt) {
    // TODO: FIX MOUSE EVENTS SO THAT IF IMGUI CONSUMES IT, IT DOES NOT CONTINUE TO GET PASSED ON TO MY EVENT LISTENERS!!!
    m_DebounceLeft -= dt;

    if (m_DebounceLeft <= 0.0f && Input::MouseButtonPressed(JADE_MOUSE_BUTTON_LEFT)) {
        float worldX = Input::OrthoMouseX();
        float worldY = Input::OrthoMouseY();

        auto view = registry.view<Transform>();
        for (auto entity : view) {
            Transform& transform = view.get<Transform>(entity);
            if (worldX >= transform.m_Position.x && worldX <= transform.m_Position.x + transform.m_Scale.x && 
                worldY >= transform.m_Position.y && worldY <= transform.m_Position.y + transform.m_Scale.y) {
                JWindow::GetScene()->SetActiveEntity(entity);
                m_DebounceLeft = m_DebounceTime;
                break;
            }
        }
    }
}

void LevelEditorSystem::ImGui(entt::registry& registry) {
    ImGui::Begin("Scene");
    int index = 0;
    auto view = registry.view<Transform>();
    
    for (auto entity : view) {
        Transform& transform = view.get<Transform>(entity);
        if (entt::to_integral(transform.m_Parent) == entt::to_integral(entt::null)) {
            DoTreeNode(index, transform, registry);
        }
    }
    ImGui::End();

    entt::entity activeEntity = JWindow::GetScene()->GetActiveEntity();
    if (registry.has<Transform>(activeEntity)) {
        Transform& transform = registry.get<Transform>(activeEntity);
        if (ImGui::CollapsingHeader("Transform")) {
            ImGui::DragFloat3("Position: ", glm::value_ptr(transform.m_Position));
            ImGui::DragFloat3("Scale: ", glm::value_ptr(transform.m_Scale));
            ImGui::DragFloat3("Rotation: ", glm::value_ptr(transform.m_EulerRotation));
        }
    }
}


void LevelEditorSystem::DoTreeNode(int& index, const Transform& transform, const entt::registry& registry) {
    std::string str = transform.m_Name;
    std::string res = str + "##" + std::to_string(index);
    index++;

    entt::entity next = transform.m_Next;
    while (entt::to_integral(next) != entt::to_integral(entt::null)) {
        const Transform& transform2 = registry.get<Transform>(next);
        std::string str2 = transform2.m_Name;
        std::string res2 = str2 + "##" + std::to_string(index);
        index++;

        if (ImGui::TreeNode(res2.c_str())) {
            if (entt::to_integral(transform2.m_First) != entt::to_integral(entt::null)) {
                const Transform& childTrans = registry.get<Transform>(transform2.m_First);
                DoTreeNode(index, childTrans, registry);
            }
            ImGui::TreePop();
        }
        next = transform2.m_Next;
    }

    if(ImGui::TreeNode(res.c_str())) {
        if (entt::to_integral(transform.m_First) != entt::to_integral(entt::null)) {
            const Transform& firstTransform = registry.get<Transform>(transform.m_First);
            DoTreeNode(index, firstTransform, registry);
        }
        ImGui::TreePop();
    }
}