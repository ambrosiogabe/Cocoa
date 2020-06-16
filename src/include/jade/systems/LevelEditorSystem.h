#pragma once

#include "jade/systems/System.h"
#include "jade/components/components.h"

class LevelEditorSystem : public System {
public:
    virtual void Update(entt::registry& registry, float dt) override;
    virtual void Start(entt::registry& registry) override;
    virtual void ImGui(entt::registry& registry) override;

private:
    void DoTreeNode(int& index, const Transform& transform, const entt::registry& registry);
};