#pragma once

#include "jade/systems/RenderSystem.h"

#include <entt/entt.h>
#include <vector>

class Scene {
public:
    virtual void Init() = 0;
    virtual void Render() = 0;
    virtual void ImGui() = 0;
    virtual void Update(float dt) = 0;
    virtual void Start() = 0;

    entt::entity GetActiveEntity() const { return m_ActiveEntity; }

    void AddSystem(System* system) {
        m_Systems[m_SystemCount++] = system;
    }

    const entt::registry& GetRegistry() const {
        return m_Registry;
    }

protected:
    System* m_Systems[100];
    uint16 m_SystemCount = 0;

    entt::registry m_Registry = entt::registry();

    Camera* m_Camera;
    entt::entity m_ActiveEntity = entt::null;
};