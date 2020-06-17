#pragma once

#include "jade/systems/System.h"
#include "jade/components/components.h"
#include "jade/commands/MoveTransformCommand.h"

class LevelEditorSystem : public System {
public:
    virtual void Update(entt::registry& registry, float dt) override;
    virtual void Start(entt::registry& registry) override;
    virtual void ImGui(entt::registry& registry) override;

private:
    void DoTreeNode(int& index, const Transform& transform, const entt::registry& registry);

private:
    float m_DebounceTime = 0.1f;
    float m_DebounceLeft = 0.0f;

    float m_KeyDebounceTime = 0.1f;
    float m_KeyDebounceLeft = 0.0f;

    bool m_IsDragging = false;

    glm::vec3 m_DragOffset = glm::vec3();
};