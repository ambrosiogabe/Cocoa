#include "jade/scenes/Scene.h"
#include "jade/file/OutputArchive.h"
#include "jade/file/IFile.h"
#include "jade/util/Settings.h"

#include <nlohmann/json.hpp>


namespace Jade
{
	void Scene::SetActiveEntity(entt::entity entity)
	{
		m_ActiveEntity = entity;
	}

	entt::registry& Scene::GetRegistry()
	{
		return m_Registry;
	}

	Camera* Scene::GetCamera() const
	{
		return m_Camera;
	}

	entt::entity Scene::GetActiveEntity() const
	{
		return m_ActiveEntity;
	}

	const std::vector<std::unique_ptr<System>>& Scene::GetSystems()
	{
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

	void Scene::Save(const JPath& filename)
	{
		m_SaveDataJson = {
			{"Size", 0},
			{"Components", {}},
			{"Project", Settings::General::s_CurrentProject.Filepath()},
			{"Assets", AssetManager::Serialize()}
		};

		OutputArchive output;
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

		int size = j["Size"].is_null() ? 0 : j["Size"];
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

		IFile::CloseFile(file);
	}
}