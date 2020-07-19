#include "jade/scenes/Scene.h"
#include "jade/file/OutputArchive.h"

#include <jsonVendor/json.hpp>


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

    void Scene::Save(const std::string& filename)
    {
        m_SaveFileStream.open(filename);
        
        m_SaveDataJson = {
            {"Size", 0},
            {"Components", {}}
        };

        OutputArchive output;
        entt::snapshot { m_Registry }
            .entities(output)
            .component<Transform, Rigidbody2D, Box2D, SpriteRenderer, AABB>(output);

        m_SaveFileStream << m_SaveDataJson.dump(4);
        m_SaveFileStream.close();
    }

    void Scene::Load(const std::string& filename)
    {
        m_Registry.clear<SpriteRenderer, Transform, Box2D, AABB, Circle, Rigidbody2D>();

        std::ifstream inputStream(filename);
        std::string str;

        std::unordered_map<int, int> idKey;

        inputStream.seekg(0, std::ios::end);
        str.reserve(inputStream.tellg());
        inputStream.seekg(0, std::ios::beg);

        str.assign((std::istreambuf_iterator<char>(inputStream)),
            std::istreambuf_iterator<char>());

        json j = json::parse(str);
        int size = j["Size"];
        for (int i = 0; i < size; i++)
        {
            if (!j["Components"][i]["SpriteRenderer"].is_null())
            {
                entt::entity entity;
                if (idKey.find(j["Components"][i]["SpriteRenderer"]["Entity"]) != idKey.end())
                {
                    entity = entt::entity(idKey[j["Components"][i]["SpriteRenderer"]["Entity"]]);
                }
                else
                {
                    entity = m_Registry.create();
                    idKey[j["Components"][i]["SpriteRenderer"]["Entity"]] = entt::to_integral(entity);
                }
                RenderSystem::Deserialize(j["Components"][i], m_Registry, entity);
            } 
            else if (!j["Components"][i]["Transform"].is_null())
            {
                entt::entity entity;
                if (idKey.find(j["Components"][i]["Transform"]["Entity"]) != idKey.end())
                {
                    entity = entt::entity(idKey[j["Components"][i]["Transform"]["Entity"]]);
                }
                else
                {
                    entity = m_Registry.create();
                    idKey[j["Components"][i]["Transform"]["Entity"]] = entt::to_integral(entity);
                }
                Transform::Deserialize(j["Components"][i], m_Registry, entity);
            }
            else if (!j["Components"][i]["Rigidbody2D"].is_null())
            {
                entt::entity entity;
                if (idKey.find(j["Components"][i]["Rigidbody2D"]["Entity"]) != idKey.end())
                {
                    entity = entt::entity(idKey[j["Components"][i]["Rigidbody2D"]["Entity"]]);
                }
                else
                {
                    entity = m_Registry.create();
                    idKey[j["Components"][i]["Rigidbody2D"]["Entity"]] = entt::to_integral(entity);
                }
                Physics2DSystem::DeserializeRigidbody2D(j["Components"][i], m_Registry, entity);
            }
            else if (!j["Components"][i]["Box2D"].is_null())
            {
                entt::entity entity;
                if (idKey.find(j["Components"][i]["Box2D"]["Entity"]) != idKey.end())
                {
                    entity = entt::entity(idKey[j["Components"][i]["Box2D"]["Entity"]]);
                }
                else
                {
                    entity = m_Registry.create();
                    idKey[j["Components"][i]["Box2D"]["Entity"]] = entt::to_integral(entity);
                }
                Physics2DSystem::DeserializeBox2D(j["Components"][i], m_Registry, entity);
            }
            else if (!j["Components"][i]["AABB"].is_null())
            {
                entt::entity entity;
                if (idKey.find(j["Components"][i]["AABB"]["Entity"]) != idKey.end())
                {
                    entity = entt::entity(idKey[j["Components"][i]["AABB"]["Entity"]]);
                }
                else
                {
                    entity = m_Registry.create();
                    idKey[j["Components"][i]["AABB"]["Entity"]] = entt::to_integral(entity);
                }
                Physics2DSystem::DeserializeAABB(j["Components"][i], m_Registry, entity);
            }
        }
    }

    std::ofstream& Scene::GetSaveFile()
    {
        return m_SaveFileStream;
    }
}