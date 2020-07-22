#pragma once
#include "externalLibs.h"

#include "jade/systems/RenderSystem.h"
#include "jade/core/ImGuiLayer.h"

namespace Jade {
    class Scene {
    public:
        virtual void Init() = 0;
        virtual void Render() = 0;
        virtual void ImGui() = 0;
        virtual void Update(float dt) = 0;
        virtual void Start() = 0;

        void Play();
        void Stop();
        void Save(const std::string& filename);
        void Load(const std::string& filename);

        void SetActiveEntity(entt::entity entity);
        entt::registry& GetRegistry();
        Camera* GetCamera() const;
        entt::entity GetActiveEntity() const;

        const std::vector<std::unique_ptr<System>>& GetSystems();

        // TODO: TEMPORARY GET BETTER SYSTEM THEN THESE!!!
        inline json& GetSaveDataJson()
        {
            return m_SaveDataJson;
        }

    public:
        template<typename T>
        std::pair<entt::registry&, entt::entity> GetEntity(const T& component)
        {
            size_t offset = &component - m_Registry.raw<T>();
            Log::Assert(offset < m_Registry.size(), "Tried to get nonexistent entity.");
            return { m_Registry, *(m_Registry.data<T>() + offset) };
        }

    protected:
        bool m_IsRunning = false;
        std::vector<std::unique_ptr<System>> m_Systems;

        entt::registry m_Registry = entt::registry();
        json m_SaveDataJson;

        Camera* m_Camera;
        entt::entity m_ActiveEntity = entt::null;
    };
}