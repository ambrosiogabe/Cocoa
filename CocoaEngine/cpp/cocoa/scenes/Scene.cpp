#include "cocoa/scenes/Scene.h"

#include "cocoa/file/OutputArchive.h"
#include "cocoa/file/IFile.h"
#include "cocoa/util/Settings.h"
#include "cocoa/core/Entity.h"
#include "cocoa/components/Transform.h"
#include "cocoa/systems/ScriptSystem.h"
#include "cocoa/scenes/SceneInitializer.h"
#include "cocoa/core/AssetManager.h"

#include <nlohmann/json.hpp>

namespace Cocoa
{
	Scene::Scene(SceneInitializer* sceneInitializer)
	{
		m_SceneInitializer = sceneInitializer;

		m_ShowDemoWindow = false;
		m_Camera = nullptr;
		m_IsPlaying = false;

		m_Registry = entt::registry();
		m_Systems = std::vector<std::unique_ptr<System>>();
	}

	Entity Scene::CreateEntity()
	{
		entt::entity e = m_Registry.create();
		Entity entity = Entity(e, this);
		entity.AddComponent<Transform>();
		return entity;
	}

	void Scene::Init()
	{
		LoadDefaultAssets();

		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0);
		m_Camera = new Camera(cameraPos);

		Physics2D::SetScene(this);
		Input::SetScene(this);
		Entity::SetScene(this);

		m_Systems.emplace_back(std::make_unique<RenderSystem>("Render System", this));
		m_Systems.emplace_back(std::make_unique<Physics2DSystem>("Physics2D System", this));
		m_Systems.emplace_back(std::make_unique<ScriptSystem>("Script System", this));
		m_SceneInitializer->Init(this, m_Systems);
	}

	void Scene::Start()
	{
		for (const auto& system : m_Systems)
		{
			system->Start();
		}
	}

	void Scene::Update(float dt)
	{
		Physics2D::Get()->Update(dt);
		for (const auto& system : m_Systems)
		{
			system->Update(dt);
		}
	}

	void Scene::EditorUpdate(float dt)
	{
		for (const auto& system : m_Systems)
		{
			system->EditorUpdate(dt);
		}
	}

	void Scene::Render()
	{
		for (const auto& system : m_Systems)
		{
			system->Render();
		}
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
		m_IsPlaying = true;
		auto view = m_Registry.view<Transform>();
		for (auto entity : view)
		{
			Physics2D::Get()->AddEntity(entity);
		}
	}

	void Scene::Stop()
	{
		m_IsPlaying = false;
		Physics2D::Get()->Destroy();
	}

	void Scene::Save(const CPath& filename)
	{
		Log::Info("Saving scene for %s", filename.Filepath());
		m_SaveDataJson = {
			{"Components", {}},
			{"Project", Settings::General::s_CurrentProject.Filepath()},
			{"Assets", AssetManager::Serialize()}
		};

		OutputArchive output(m_SaveDataJson);
		entt::snapshot{ m_Registry }
			.entities(output)
			.component<Transform, Rigidbody2D, Box2D, SpriteRenderer, FontRenderer, AABB>(output);

		for (const auto& system : m_Systems)
		{
			if (strcmp(system.get()->GetName(), "Script System") == 0)
			{
				ScriptSystem* scriptSystem = (ScriptSystem*)system.get();
				scriptSystem->SaveScripts(m_SaveDataJson);
				break;
			}
		}

		IFile::WriteFile(m_SaveDataJson.dump(4).c_str(), filename);
	}

	void Scene::Reset()
	{
		AssetManager::Clear();
		auto view = m_Registry.view<Transform>();
		m_Registry.destroy(view.begin(), view.end());

		for (auto& system : m_Systems)
		{
			if (strcmp(system->GetName(), "Script System") == 0)
			{
				ScriptSystem* scriptSystem = (ScriptSystem*)system.get();
				scriptSystem->FreeScriptLibrary();
				break;
			}
		}

		m_Systems.clear();
		delete m_Camera;
		Init();
	}

	void Scene::LoadDefaultAssets()
	{
		auto asset = AssetManager::LoadTextureFromFile(Settings::General::s_EngineAssetsPath + "images/gizmos.png", true);
	}

	static Entity FindOrCreateEntity(int id, Scene* scene, entt::registry& registry)
	{
		Entity entity;
		if (registry.valid(entt::entity(id)))
		{
			entity = Entity(entt::entity(id), scene);
		}
		else
		{
			entity = Entity(registry.create(entt::entity(id)), scene);
		}

		return entity;
	}

	void Scene::Load(const CPath& filename)
	{
		Reset();
		Log::Info("Loading scene %s", filename.Filepath());

		Settings::General::s_CurrentScene = filename;
		File* file = IFile::OpenFile(filename);
		if (file->m_Data.size() <= 0)
		{
			return;
		}

		ScriptSystem* scriptSystem = nullptr;
		for (auto& system : m_Systems)
		{
			system->Start();
			if (strcmp(system->GetName(), "Script System") == 0)
			{
				scriptSystem = (ScriptSystem*)system.get();
			}
		}

		json j = json::parse(file->m_Data);

		if (j.contains("Assets"))
		{
			AssetManager::LoadTexturesFrom(j["Assets"]);
			AssetManager::LoadFontsFrom(j["Assets"]);
		}

		int size = !j.contains("Components") ? 0 : j["Components"].size();
		for (int i=0; i < size; i++)
		{
			json::iterator it = j["Components"][i].begin();
			json component = j["Components"][i];
			if (it.key() == "SpriteRenderer")
			{
				Entity entity = FindOrCreateEntity(component["SpriteRenderer"]["Entity"], this, m_Registry);
				RenderSystem::DeserializeSpriteRenderer(component, entity);
			}
			else if (it.key() == "FontRenderer")
			{
				Entity entity = FindOrCreateEntity(component["FontRenderer"]["Entity"], this, m_Registry);
				RenderSystem::DeserializeFontRenderer(component, entity);
			}
			else if (it.key() == "Transform")
			{
				Entity entity = FindOrCreateEntity(component["Transform"]["Entity"], this, m_Registry);
				Transform::Deserialize(component, entity);
			}
			else if (it.key() == "Rigidbody2D")
			{
				Entity entity = FindOrCreateEntity(component["Rigidbody2D"]["Entity"], this, m_Registry);
				Physics2DSystem::DeserializeRigidbody2D(component, entity);
			}
			else if (it.key() == "Box2D")
			{
				Entity entity = FindOrCreateEntity(component["Box2D"]["Entity"], this, m_Registry);
				Physics2DSystem::DeserializeBox2D(component, entity);
			}
			else if (it.key() == "AABB")
			{
				Entity entity = FindOrCreateEntity(component["AABB"]["Entity"], this, m_Registry);
				Physics2DSystem::DeserializeAABB(component, entity);
			}
			else
			{
				Entity entity = FindOrCreateEntity(component.front()["Entity"], this, m_Registry);
				scriptSystem->Deserialize(component, entity);
			}
		}

		IFile::CloseFile(file);
	}

	void Scene::LoadScriptsOnly(const CPath& filename)
	{
		File* file = IFile::OpenFile(filename);
		if (file->m_Data.size() <= 0)
		{
			return;
		}

		ScriptSystem* scriptSystem = nullptr;
		for (auto& system : m_Systems)
		{
			if (strcmp(system->GetName(), "Script System") == 0)
			{
				scriptSystem = (ScriptSystem*)system.get();
			}
		}

		Log::Info("Loading scripts only for %s", filename.Filepath());
		json j = json::parse(file->m_Data);
		int size = !j.contains("Components") ? 0 : j["Components"].size();
		for (int i = 0; i < size; i++)
		{
			json::iterator it = j["Components"][i].begin();
			json component = j["Components"][i];
			if (it.key() != "SpriteRenderer" && it.key() != "Transform" && it.key() != "Rigidbody2D" && it.key() != "Box2D" && it.key() != "AABB")
			{
				Entity entity = FindOrCreateEntity(component.front()["Entity"], this, m_Registry);
				scriptSystem->Deserialize(component, entity);
			}
		}

		IFile::CloseFile(file);
	}
}