#include "jade/scenes/Scene.h"

namespace Jade {    
    void Scene::SetActiveEntity(entt::entity entity) { 
        m_ActiveEntity = entity; 
    }

    entt::registry& Scene::GetRegistry() { 
        return m_Registry; 
    }

    Camera* Scene::GetCamera() const { 
        return m_Camera;
    }

    entt::entity Scene::GetActiveEntity() const { 
        return m_ActiveEntity; 
    }

    const std::vector<std::unique_ptr<System>>& Scene::GetSystems() {
        return m_Systems;
    }

    void Scene::Play()
    {
        auto view = m_Registry.view<Transform>();
        for (auto entity : view)
        {
            Physics2D::Get()->AddEntity(entity);
        }

        m_IsRunning = true;
    }

    void Scene::Stop()
    {
        Physics2D::Get()->Destroy();
        m_IsRunning = false;
    }
}