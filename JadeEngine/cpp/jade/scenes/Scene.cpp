#include "jade/scenes/Scene.h"
#include "jade/file/OutputArchive.h"
#include "jade/file/IFile.h"
#include "jade/util/Settings.h"
#include "jade/core/Entity.h"
#include "jade/components/Transform.h"

#include <nlohmann/json.hpp>


namespace Jade
{
	Entity Scene::CreateEntity()
	{
		entt::entity e = m_Registry.create();
		Entity entity = Entity(e, this);
		entity.AddComponent<Transform>();
		return entity;
	}

	Entity Scene::GetEntity(uint32 id)
	{
		entt::entity entity = entt::null;
		if (id < std::numeric_limits<uint32>::max())
		{
			entity = entt::entity(id);
		}
		return Entity(entity, this);
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

	void Scene::Save(const JPath& filename)
	{
		m_SaveDataJson = {
			{"Size", 0},
			{"Components", {}},
			{"Project", Settings::General::s_CurrentProject.Filepath()},
			{"Assets", AssetManager::Serialize()}
		};

		OutputArchive output(m_SaveDataJson);
		entt::snapshot{ m_Registry }
			.entities(output)
			.component<Transform, Rigidbody2D, Box2D, SpriteRenderer, AABB>(output);

		IFile::WriteFile(m_SaveDataJson.dump(4).c_str(), filename);
	}

	void Scene::Reset()
	{
		for (auto entity : m_Registry.view<Transform>())
		{
			m_Registry.remove_all(entity);
		}
	}

	void Scene::LoadDefaultAssets()
	{
		AssetManager::LoadTextureFromFile(Settings::General::s_EngineAssetsPath + "images/gizmos.png", 0, true);
	}

	static Entity FindOrCreateEntity(std::unordered_map<int, int>& idKey, int id, Scene* scene, entt::registry& registry)
	{
		Entity entity;
		if (idKey.find(id) != idKey.end())
		{
			entity = Entity(entt::entity(idKey[id]), scene);
		}
		else
		{
			entity = Entity(registry.create(), scene);
			idKey[id] = entity.GetID();
		}

		return entity;
	}

	void Scene::Load(const JPath& filename)
	{
		Reset();

		File* file = IFile::OpenFile(filename);
		if (file->m_Data.size() <= 0)
		{
			return;
		}

		std::unordered_map<int, int> idKey;
		json j = json::parse(file->m_Data);

		if (!j["Assets"].is_null())
		{
			AssetManager::LoadFrom(j["Assets"]);
			LoadDefaultAssets();
		}

		int size = j["Size"].is_null() ? 0 : j["Size"];
		for (int i = 0; i < size; i++)
		{
			if (!j["Components"][i]["SpriteRenderer"].is_null())
			{
				Entity entity = FindOrCreateEntity(idKey, j["Components"][i]["SpriteRenderer"]["Entity"], this, m_Registry);
				RenderSystem::Deserialize(j["Components"][i], entity);
			}
			else if (!j["Components"][i]["Transform"].is_null())
			{
				Entity entity = FindOrCreateEntity(idKey, j["Components"][i]["Transform"]["Entity"], this, m_Registry);
				Transform::Deserialize(j["Components"][i], entity);
			}
			else if (!j["Components"][i]["Rigidbody2D"].is_null())
			{
				Entity entity = FindOrCreateEntity(idKey, j["Components"][i]["Rigidbody2D"]["Entity"], this, m_Registry);
				Physics2DSystem::DeserializeRigidbody2D(j["Components"][i], entity);
			}
			else if (!j["Components"][i]["Box2D"].is_null())
			{
				Entity entity = FindOrCreateEntity(idKey, j["Components"][i]["Box2D"]["Entity"], this, m_Registry);
				Physics2DSystem::DeserializeBox2D(j["Components"][i], entity);
			}
			else if (!j["Components"][i]["AABB"].is_null())
			{
				Entity entity = FindOrCreateEntity(idKey, j["Components"][i]["AABB"]["Entity"], this, m_Registry);
				Physics2DSystem::DeserializeAABB(j["Components"][i], entity);
			}
		}

		IFile::CloseFile(file);
	}
}