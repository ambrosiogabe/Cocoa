#pragma once

#include "jade/systems/RenderSystem.h"
#include "jade/core/ImGuiLayer.h"

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

        void SetActiveEntity(entt::entity entity);
        const glm::vec2& GetGameviewPos() const;
        const glm::vec2& GetGameviewSize() const;
        entt::registry& GetRegistry();
        Camera* GetCamera() const;
        entt::entity GetActiveEntity() const;

        const std::vector<std::unique_ptr<System>>& GetSystems();

    protected:
        ImGuiLayer m_ImGuiLayer = ImGuiLayer();

        std::vector<std::unique_ptr<System>> m_Systems;

        entt::registry m_Registry = entt::registry();

        Camera* m_Camera;
        entt::entity m_ActiveEntity = entt::null;
    };
}