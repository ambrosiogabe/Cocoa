#include "jade/scenes/Scene.h"
#include "jade/file/OutputArchive.h"
#include "jade/file/IFile.h"
#include "jade/util/Settings.h"
#include "jade/core/Entity.h"
#include "jade/components/Transform.h"

#include <nlohmann/json.hpp>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

namespace Jade
{
	Entity Scene::CreateEntity()
	{
		entt::entity e = m_Registry.create();
		Entity entity = Entity(e, this);
		entity.AddComponent<Transform>();
		return entity;
	}

	Entity Scene::DuplicateEntity(Entity entity)
	{
		entt::entity newEntEntity = m_Registry.create();
		Entity newEntity = Entity(newEntEntity, this);
		if (entity.HasComponent<Transform>())
		{
			newEntity.AddComponent<Transform>(entity.GetComponent<Transform>());
		}

		if (entity.HasComponent<SpriteRenderer>())
		{
			newEntity.AddComponent<SpriteRenderer>(entity.GetComponent<SpriteRenderer>());
		}

		if (entity.HasComponent<Rigidbody2D>())
		{
			newEntity.AddComponent<Rigidbody2D>(entity.GetComponent<Rigidbody2D>());
		}

		if (entity.HasComponent<Box2D>())
		{
			newEntity.AddComponent<Box2D>(entity.GetComponent<Box2D>());
		}

		return newEntity;
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
		AssetManager::Clear();
		for (auto entity : m_Registry.view<Transform>())
		{
			m_Registry.remove_all(entity);
		}
		m_Systems.clear();
		delete m_Camera;
		Init();
	}

	void Scene::LoadDefaultAssets()
	{
		auto asset = AssetManager::LoadTextureFromFile(Settings::General::s_EngineAssetsPath + "images/gizmos.png", true);
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

		//mono_set_dirs("C:\\dev\\C++\\MonoTEST\\MonoTest\\monoVendor\\lib", "C:\\dev\\C++\\MonoTEST\\MonoTest\\monoVendor\\etc");

		//MonoDomain* domain = mono_jit_init("JadeEngineScriptRuntime");
		//JPath testFile = "C:/dev/C#/Pacman/bin/Windows/x86/Debug/Pacman.exe";
		//MonoImageOpenStatus status;
		//MonoAssembly* assembly = mono_assembly_open("C:\\dev\\C++\\MonoTEST\\MonoTest\\Art.exe", &status);
		//if (!assembly)
		//{
		//	Log::Error("Failed to load mono file: %s", testFile.Filepath());
		//}

		//int argc = 1;
		//char* argv[1] = { (char*)"MyDomain" };
		//int returnVal = mono_jit_exec(domain, assembly, argc, argv);
		//Log::Info("Execution of the script yielded: %d", returnVal);

		Reset();

		Settings::General::s_CurrentScene = filename;
		File* file = IFile::OpenFile(filename);
		if (file->m_Data.size() <= 0)
		{
			return;
		}

		std::unordered_map<int, int> idKey;
		std::unordered_map<uint32, uint32> resourceIdMap{};
		json j = json::parse(file->m_Data);

		if (!j["Assets"].is_null())
		{
			resourceIdMap = AssetManager::LoadFrom(j["Assets"]);
		}

		int size = j["Size"].is_null() || j["Components"].is_null() ? 0 : j["Size"];
		for (int i = 0; i < size; i++)
		{
			if (!j["Components"][i]["SpriteRenderer"].is_null())
			{
				Entity entity = FindOrCreateEntity(idKey, j["Components"][i]["SpriteRenderer"]["Entity"], this, m_Registry);
				const json& originalId = j["Components"][i]["SpriteRenderer"]["AssetId"];
				if (!originalId.is_null() && (uint32)originalId != std::numeric_limits<uint32>::max())
					j["Components"][i]["SpriteRenderer"]["AssetId"] = resourceIdMap[(uint32)originalId];
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