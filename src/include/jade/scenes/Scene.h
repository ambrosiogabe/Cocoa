#pragma once

#include "jade/systems/RenderSystem.h"
#include "jade/core/ImGuiLayer.h"

#include <entt/entt.h>
#include <vector>

class Scene {
public:
    virtual void Init() = 0;
    virtual void Render() = 0;
    virtual void ImGui() = 0;
    virtual void Update(float dt) = 0;
    virtual void Start() = 0;


    void AddSystem(System* system) { m_Systems[m_SystemCount++] = system; }
    
    void SetActiveEntity(entt::entity entity) { m_ActiveEntity = entity; }

    const glm::vec2& GetGameviewPos() const { return m_ImGuiLayer.GetGameviewPos(); }
    const glm::vec2& GetGameviewSize() const { return m_ImGuiLayer.GetGameviewSize(); }
    entt::registry&  GetRegistry() { return m_Registry; }
    Camera*          GetCamera() const { return m_Camera;}
    entt::entity     GetActiveEntity() const { return m_ActiveEntity; }

protected:
    ImGuiLayer m_ImGuiLayer = ImGuiLayer();

    System* m_Systems[100];
    uint16 m_SystemCount = 0;

    entt::registry m_Registry = entt::registry();

    Camera* m_Camera;
    entt::entity m_ActiveEntity = entt::null;
};