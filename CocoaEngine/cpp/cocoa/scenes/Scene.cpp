#include "cocoa/scenes/Scene.h"

#include "cocoa/file/OutputArchive.h"
#include "cocoa/file/File.h"
#include "cocoa/util/Settings.h"
#include "cocoa/core/Entity.h"
#include "cocoa/components/Transform.h"
#include "cocoa/components/Tag.h"
#include "cocoa/components/NoSerialize.h"
#include "cocoa/systems/ScriptSystem.h"
#include "cocoa/systems/TransformSystem.h"
#include "cocoa/scenes/SceneInitializer.h"
#include "cocoa/core/AssetManager.h"
#include "cocoa/renderer/DebugDraw.h"

#include <nlohmann/json.hpp>

namespace Cocoa
{
	namespace Scene
	{
		// Forward Declarations
		static void LoadDefaultAssets();
		static Entity FindOrCreateEntity(int id, SceneData& scene, entt::registry& registry);

		SceneData Create(SceneInitializer* sceneInitializer)
		{
			SceneData data;
			data.CurrentSceneInitializer = sceneInitializer;
			data.IsPlaying = false;

			data.Registry = entt::registry();
			return data;
		}

		static void InitComponentIds(SceneData& scene);
		void Init(SceneData& data)
		{
			NEntity::setScene(&data);

			InitComponentIds(data);
			LoadDefaultAssets();

			RenderSystem::Init();
			Physics2D::init({ 0, -10.0f });
			ScriptSystem::Init(data);

			data.CurrentSceneInitializer->Init(data);
		}

		static void InitComponentIds(SceneData& scene)
		{
			NEntity::registerComponentType<TransformData>();
			NEntity::registerComponentType<Tag>();
			NEntity::registerComponentType<SpriteRenderer>();
			NEntity::registerComponentType<FontRenderer>();
			NEntity::registerComponentType<Rigidbody2D>();
			NEntity::registerComponentType<Box2D>();
			NEntity::registerComponentType<Circle>();
			NEntity::registerComponentType<AABB>();
			NEntity::registerComponentType<Camera>();
			NEntity::registerComponentType<NoSerialize>();
		}

		void Start(SceneData& data)
		{
			data.CurrentSceneInitializer->Start(data);
		}

		void Update(SceneData& data, float dt)
		{
			TransformSystem::update(data, dt);
			Physics2D::update(data, dt);
			ScriptSystem::Update(data, dt);
			CameraSystem::update(data, dt);
		}

		void EditorUpdate(SceneData& data, float dt)
		{
			// There are certain systems that use the same update loop for the editor and the actual game, so there's no 
			// sense in creating a unique update loop if the logic is the same (TransformSystem, and NCamera are examples of this)
			TransformSystem::update(data, dt);
			ScriptSystem::EditorUpdate(data, dt);
			CameraSystem::update(data, dt);
		}

		void OnEvent(SceneData& data, const Event& e)
		{
			// TODO: If you have systems that require events, place them in here
		}

		void Render(SceneData& data)
		{
			RenderSystem::Render(data);
		}

		void FreeResources(SceneData& data)
		{
			Logger::Log("Freeing scene resources"); 
			AssetManager::clear();

			TransformSystem::Destroy(data);
			RenderSystem::Destroy();
			Physics2D::destroy(data);
			CameraSystem::destroy(data);

			data.CurrentSceneInitializer->Destroy(data);

			// Make sure to clear the entities last, that way we can properly destroy all
			// the instances in the other systems if needed
			data.Registry.clear();
			data.Registry = entt::registry();

			ScriptSystem::FreeScriptLibrary(data);
		}

		void Play(SceneData& data)
		{
			data.IsPlaying = true;
			auto view = data.Registry.view<TransformData>();
			for (auto entity : view)
			{
				Physics2D::addEntity(Entity{ entity });
			}
		}

		void Stop(SceneData& data)
		{
			data.IsPlaying = false;
		}

		void SerializeEntity(json* j, Entity entity)
		{
			Logger::Assert(j->contains("Components"), "Cannot deserialize entity in json object that does not have components array");

			// Only serialize if we can
			if (NEntity::hasComponent<TransformData>(entity))
			{
				Transform::serialize(*j, entity, NEntity::getComponent<TransformData>(entity));
			}
			if (NEntity::hasComponent<SpriteRenderer>(entity))
			{
				RenderSystem::Serialize(*j, entity, NEntity::getComponent<SpriteRenderer>(entity));
			}
			if (NEntity::hasComponent<FontRenderer>(entity))
			{
				RenderSystem::Serialize(*j, entity, NEntity::getComponent<FontRenderer>(entity));
			}
			if (NEntity::hasComponent<Box2D>(entity))
			{
				Physics2D::serialize(*j, entity, NEntity::getComponent<Box2D>(entity));
			}
			if (NEntity::hasComponent<Rigidbody2D>(entity))
			{
				Physics2D::serialize(*j, entity, NEntity::getComponent<Rigidbody2D>(entity));
			}
			if (NEntity::hasComponent<AABB>(entity))
			{
				Physics2D::serialize(*j, entity, NEntity::getComponent<AABB>(entity));
			}
			if (NEntity::hasComponent<Tag>(entity))
			{
				NTag::serialize(*j, entity, NEntity::getComponent<Tag>(entity));
			}
			if (NEntity::hasComponent<Camera>(entity))
			{
				NCamera::serialize(j, entity, NEntity::getComponent<Camera>(entity));
			}
		}

		void DeserializeEntities(const json& j, SceneData& scene)
		{
			if (!j.contains("Components"))
			{
				Logger::Warning("Tried to deserialize entities on json that did not contain any valid entities.");
			}

			int size = !j.contains("Components") ? 0 : j["Components"].size();
			for (int i = 0; i < size; i++)
			{
				if (!j["Components"][i].is_object())
				{
					Logger::Warning("Skipped array element when deserializing entities because it was not a valid json object.");
					continue;
				}

				json::const_iterator it = j["Components"][i].begin();
				const json component = j["Components"][i];
				if (it.key() == "SpriteRenderer")
				{
					Entity entity = FindOrCreateEntity(component["SpriteRenderer"]["Entity"], scene, scene.Registry);
					RenderSystem::DeserializeSpriteRenderer(component, entity);
				}
				else if (it.key() == "FontRenderer")
				{
					Entity entity = FindOrCreateEntity(component["FontRenderer"]["Entity"], scene, scene.Registry);
					RenderSystem::DeserializeFontRenderer(component, entity);
				}
				else if (it.key() == "Transform")
				{
					Entity entity = FindOrCreateEntity(component["Transform"]["Entity"], scene, scene.Registry);
					Entity parentEntity = FindOrCreateEntity(component["Transform"]["Parent"], scene, scene.Registry);
					Transform::deserialize(component, entity, parentEntity);
				}
				else if (it.key() == "Rigidbody2D")
				{
					Entity entity = FindOrCreateEntity(component["Rigidbody2D"]["Entity"], scene, scene.Registry);
					Physics2D::deserializeRigidbody2D(component, entity);
				}
				else if (it.key() == "Box2D")
				{
					Entity entity = FindOrCreateEntity(component["Box2D"]["Entity"], scene, scene.Registry);
					Physics2D::deserializeBox2D(component, entity);
				}
				else if (it.key() == "AABB")
				{
					Entity entity = FindOrCreateEntity(component["AABB"]["Entity"], scene, scene.Registry);
					Physics2D::deserializeAabb(component, entity);
				}
				else if (it.key() == "Tag")
				{
					Entity entity = FindOrCreateEntity(component["Tag"]["Entity"], scene, scene.Registry);
					NTag::deserialize(component, entity);
				}
				else if (it.key() == "Camera")
				{
					Entity entity = FindOrCreateEntity(component["Camera"]["Entity"], scene, scene.Registry);
					NCamera::deserialize(component, entity);
				}
				else
				{
					Entity entity = FindOrCreateEntity(component.front()["Entity"], scene, scene.Registry);
					ScriptSystem::Deserialize(scene, component, entity);
				}
			}
		}

		void Save(SceneData& data, const CPath& filename)
		{
			Logger::Log("Saving scene '%s'", filename.path);
			data.SaveDataJson = {
				{"Components", {}},
				{"Project", Settings::General::s_CurrentProject.path},
				{"Assets", AssetManager::serialize()}
			};

			data.Registry.each([&](auto rawEntity)
				{
					Entity entity = NEntity::createEntity(rawEntity);
					// Only serialize if we can
					if (!NEntity::hasComponent<NoSerialize>(entity))
					{
						SerializeEntity(&data.SaveDataJson, entity);
					}
				}
			);

			ScriptSystem::SaveScripts(data, data.SaveDataJson);
			data.CurrentSceneInitializer->Save(data);

			File::writeFile(data.SaveDataJson.dump(4).c_str(), filename);
		}

		void Load(SceneData& data, const CPath& filename, bool setAsCurrentScene)
		{
			Logger::Log("Loading scene %s", filename.path);
			Init(data);

			if (setAsCurrentScene)
			{
				Settings::General::s_CurrentScene = filename;
			}

			FileHandle* file = File::openFile(filename);
			if (file->size <= 0)
			{
				File::closeFile(file);
				return;
			}


			json j = json::parse(file->data);
			// TODO: Change this so that the scene doesn't hold the json at all
			data.SaveDataJson = j;

			if (j.contains("Assets"))
			{
				AssetManager::loadTexturesFrom(j["Assets"]);
				AssetManager::loadFontsFrom(j["Assets"]);
			}

			DeserializeEntities(j, data);

			// After we deserialize the entities, hand it off to the application in case they saved anything as well
			data.CurrentSceneInitializer->Load(data);

			j = {};
			File::closeFile(file);
		}

		void LoadScriptsOnly(SceneData& data, const CPath& filename)
		{
			FileHandle* file = File::openFile(filename);
			if (file->size <= 0)
			{
				return;
			}

			Logger::Info("Loading scripts only for %s", filename.path);
			json j = json::parse(file->data);
			int size = !j.contains("Components") ? 0 : j["Components"].size();
			for (int i = 0; i < size; i++)
			{
				json::iterator it = j["Components"][i].begin();
				json component = j["Components"][i];
				if (it.key() != "SpriteRenderer" && it.key() != "Transform" && it.key() != "Rigidbody2D" && it.key() != "Box2D" && it.key() != "AABB" && it.key() != "Tag"
					&& it.key() != "Camera")
				{
					Entity entity = FindOrCreateEntity(component.front()["Entity"], data, data.Registry);
					ScriptSystem::Deserialize(data, component, entity);
				}
			}

			j = {};
			File::closeFile(file);
		}

		Entity CreateEntity(SceneData& data)
		{
			entt::entity e = data.Registry.create();
			Entity entity = Entity{ e };
			// TODO: Make transform's optional with entities
			TransformData defaultTransform = Transform::createTransform();
			NEntity::addComponent<TransformData>(entity, defaultTransform);
			NEntity::addComponent<Tag>(entity, NTag::createTag("New Entity"));
			return entity;
		}

		Entity DuplicateEntity(SceneData& data, Entity entity)
		{
			entt::entity newEntEntity = data.Registry.create();
			Entity newEntity = Entity{ newEntEntity };
			if (NEntity::hasComponent<TransformData>(entity))
			{
				NEntity::addComponent<TransformData>(newEntity, NEntity::getComponent<TransformData>(entity));
			}

			if (NEntity::hasComponent<SpriteRenderer>(entity))
			{
				NEntity::addComponent<SpriteRenderer>(newEntity, NEntity::getComponent<SpriteRenderer>(entity));
			}

			if (NEntity::hasComponent<Rigidbody2D>(entity))
			{
				NEntity::addComponent<Rigidbody2D>(newEntity, NEntity::getComponent<Rigidbody2D>(entity));
			}

			if (NEntity::hasComponent<Box2D>(entity))
			{
				NEntity::addComponent<Box2D>(newEntity, NEntity::getComponent<Box2D>(entity));
			}

			if (NEntity::hasComponent<Tag>(entity))
			{
				Tag& tag = NEntity::getComponent<Tag>(entity);
				char* newTagName = (char*)AllocMem((tag.size + 1) * sizeof(char));
				memcpy(newTagName, tag.name, (tag.size + 1) * sizeof(char));
				NEntity::addComponent<Tag>(newEntity, NTag::createTag(newTagName, true));
			}

			return newEntity;
		}

		Entity GetEntity(SceneData& data, uint32 id)
		{
			entt::entity entity = entt::null;
			if (id < std::numeric_limits<uint32>::max())
			{
				entity = entt::entity(id);
			}
			return Entity{ entity };
		}

		bool IsValid(SceneData& scene, uint32 entityId)
		{
			return scene.Registry.valid(entt::entity(entityId));
		}

		bool IsValid(SceneData& scene, Entity entity)
		{
			return scene.Registry.valid(entity.handle);
		}

		void DeleteEntity(SceneData& scene, Entity entity)
		{
			// Recursively delete entity and all children
			auto view = scene.Registry.view<TransformData>();
			for (entt::entity rawEntity : view)
			{
				Entity potentialChild = NEntity::createEntity(rawEntity);
				TransformData& transformData = NEntity::getComponent<TransformData>(potentialChild);
				if (transformData.parent == entity)
				{
					DeleteEntity(scene, potentialChild);
				}
			}

			Physics2D::deleteEntity(entity);
			TransformSystem::DeleteEntity(entity);
			CameraSystem::deleteEntity(entity);
			scene.Registry.destroy(entity.handle);
		}

		static void LoadDefaultAssets()
		{
			Texture gizmoSpec;
			gizmoSpec.magFilter = FilterMode::Linear;
			gizmoSpec.minFilter = FilterMode::Linear;
			gizmoSpec.wrapS = WrapMode::Repeat;
			gizmoSpec.wrapT = WrapMode::Repeat;
			gizmoSpec.isDefault = true;
			CPath gizmoPath = Settings::General::s_EngineAssetsPath;
			gizmoPath.join(CPath::create("images/gizmos.png"));
			auto asset = AssetManager::loadTextureFromFile(gizmoSpec, gizmoPath);
		}

		static Entity FindOrCreateEntity(int id, SceneData& scene, entt::registry& registry)
		{
			Entity entity;
			if (entt::entity(id) == entt::null)
			{
				return NEntity::createNull();
			}

			if (registry.valid(entt::entity(id)))
			{
				entity = Entity{ entt::entity(id) };
			}
			else
			{
				entity = Entity{ registry.create(entt::entity(id)) };
			}

			return entity;
		}
	}
}