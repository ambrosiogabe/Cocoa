#include "jade/systems/LevelEditorSystem.h"

#include <string>
#include <imgui/imgui.h>

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