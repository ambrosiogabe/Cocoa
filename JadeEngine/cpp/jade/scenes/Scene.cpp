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
}