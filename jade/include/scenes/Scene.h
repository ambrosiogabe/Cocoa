#pragma once

#include "systems/RenderSystem.h"
#include "core/ImGuiLayer.h"

#include <entt/entt.h>
#include <vector>

namespace Jade {
    class Scene {
    public:
        virtual void Init() = 0;
        virtual void Render() = 0;
        virtual void ImGui() = 0;
        virtual void Update(float dt) = 0;
        virtual void Start() = 0;

        void AddSystem(System* system);
        void SetActiveEntity(entt::entity entity);
        const glm::vec2& GetGameviewPos() const;
        const glm::vec2& GetGameviewSize() const;
        entt::registry& GetRegistry();
        Camera* GetCamera() const;
        entt::entity GetActiveEntity() const;

    protected:
        ImGuiLayer m_ImGuiLayer = ImGuiLayer();

        System* m_Systems[100];
        uint16 m_SystemCount = 0;

        entt::registry m_Registry = entt::registry();

        Camera* m_Camera;
        entt::entity m_ActiveEntity = entt::null;
    };
}