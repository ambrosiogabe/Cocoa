#pragma once

#include "jade/systems/RenderSystem.h"

#include <entt/entt.h>
#include <vector>

class Scene {
public:
    virtual void Init() = 0;
    virtual void Render() = 0;
    virtual void Update(float dt) = 0;
    virtual void Start() = 0;

protected:
    entt::registry m_Registry;
    RenderSystem* m_RenderSystem;
    Camera* m_Camera;
};