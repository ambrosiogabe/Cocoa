#pragma once

#include <entt/entt.h>

class System {
public:
    virtual void Start(entt::registry& registry) {

    }

    virtual void Update(entt::registry& registry, float dt) {

    }

    virtual void Render(entt::registry& registry) {

    }

    virtual void ImGui(entt::registry& registry) {
        
    }
};