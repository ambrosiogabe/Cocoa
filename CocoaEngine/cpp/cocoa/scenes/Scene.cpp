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

		SceneData create(SceneInitializer* sceneInitializer)
		{
			SceneData data;
			data.currentSceneInitializer = sceneInitializer;
			data.isPlaying = false;

			data.registry = entt::registry();
			return data;
		}

		static void InitComponentIds(SceneData& scene);
		void init(SceneData& data)
		{
			NEntity::setScene(&data);

			InitComponentIds(data);
			LoadDefaultAssets();

			RenderSystem::init();
			Physics2D::init({ 0, -10.0f });
			ScriptSystem::init(data);

			data.currentSceneInitializer->init(data);
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

		void start(SceneData& data)
		{
			data.currentSceneInitializer->start(data);
		}

		void update(SceneData& data, float dt)
		{
			TransformSystem::update(data, dt);
			Physics2D::update(data, dt);
			ScriptSystem::update(data, dt);
			CameraSystem::update(data, dt);
		}

		void editorUpdate(SceneData& data, float dt)
		{
			// There are certain systems that use the same update loop for the editor and the actual game, so there's no 
			// sense in creating a unique update loop if the logic is the same (TransformSystem, and NCamera are examples of this)
			TransformSystem::update(data, dt);
			ScriptSystem::editorUpdate(data, dt);
			CameraSystem::update(data, dt);
		}

		void onEvent(SceneData& data, const Event& e)
		{
			// TODO: If you have systems that require events, place them in here
		}

		void render(SceneData& data)
		{
			RenderSystem::render(data);
		}

		void freeResources(SceneData& data)
		{
			Logger::Log("Freeing scene resources"); 
			AssetManager::clear();

			TransformSystem::destroy(data);
			RenderSystem::destroy();
			Physics2D::destroy(data);
			CameraSystem::destroy(data);

			data.currentSceneInitializer->destroy(data);

			// Make sure to clear the entities last, that way we can properly destroy all
			// the instances in the other systems if needed
			data.registry.clear();
			data.registry = entt::registry();

			ScriptSystem::freeScriptLibrary(data);
		}

		void play(SceneData& data)
		{
			data.isPlaying = true;
			auto view = data.registry.view<TransformData>();
			for (auto entity : view)
			{
				Physics2D::addEntity(Entity{ entity });
			}
		}

		void stop(SceneData& data)
		{
			data.isPlaying = false;
		}

		void serializeEntity(json* j, Entity entity)
		{
			Logger::Assert(j->contains("Components"), "Cannot deserialize entity in json object that does not have components array");

			// Only serialize if we can
			if (NEntity::hasComponent<TransformData>(entity))
			{
				Transform::serialize(*j, entity, NEntity::getComponent<TransformData>(entity));
			}
			if (NEntity::hasComponent<SpriteRenderer>(entity))
			{
				RenderSystem::serialize(*j, entity, NEntity::getComponent<SpriteRenderer>(entity));
			}
			if (NEntity::hasComponent<FontRenderer>(entity))
			{
				RenderSystem::serialize(*j, entity, NEntity::getComponent<FontRenderer>(entity));
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

		void deserializeEntities(const json& j, SceneData& scene)
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
					Entity entity = FindOrCreateEntity(component["SpriteRenderer"]["Entity"], scene, scene.registry);
					RenderSystem::deserializeSpriteRenderer(component, entity);
				}
				else if (it.key() == "FontRenderer")
				{
					Entity entity = FindOrCreateEntity(component["FontRenderer"]["Entity"], scene, scene.registry);
					RenderSystem::deserializeFontRenderer(component, entity);
				}
				else if (it.key() == "Transform")
				{
					Entity entity = FindOrCreateEntity(component["Transform"]["Entity"], scene, scene.registry);
					Entity parentEntity = FindOrCreateEntity(component["Transform"]["Parent"], scene, scene.registry);
					Transform::deserialize(component, entity, parentEntity);
				}
				else if (it.key() == "Rigidbody2D")
				{
					Entity entity = FindOrCreateEntity(component["Rigidbody2D"]["Entity"], scene, scene.registry);
					Physics2D::deserializeRigidbody2D(component, entity);
				}
				else if (it.key() == "Box2D")
				{
					Entity entity = FindOrCreateEntity(component["Box2D"]["Entity"], scene, scene.registry);
					Physics2D::deserializeBox2D(component, entity);
				}
				else if (it.key() == "AABB")
				{
					Entity entity = FindOrCreateEntity(component["AABB"]["Entity"], scene, scene.registry);
					Physics2D::deserializeAabb(component, entity);
				}
				else if (it.key() == "Tag")
				{
					Entity entity = FindOrCreateEntity(component["Tag"]["Entity"], scene, scene.registry);
					NTag::deserialize(component, entity);
				}
				else if (it.key() == "Camera")
				{
					Entity entity = FindOrCreateEntity(component["Camera"]["Entity"], scene, scene.registry);
					NCamera::deserialize(component, entity);
				}
				else
				{
					Entity entity = FindOrCreateEntity(component.front()["Entity"], scene, scene.registry);
					ScriptSystem::deserialize(scene, component, entity);
				}
			}
		}

		void save(SceneData& data, const std::filesystem::path& filename)
		{
			Logger::Log("Saving scene '%s'", filename.string().c_str());
			data.saveDataJson = {
				{"Components", {}},
				{"Project", Settings::General::currentProject.string()},
				{"Assets", AssetManager::serialize()}
			};

			data.registry.each([&](auto rawEntity)
				{
					Entity entity = NEntity::createEntity(rawEntity);
					// Only serialize if we can
					if (!NEntity::hasComponent<NoSerialize>(entity))
					{
						serializeEntity(&data.saveDataJson, entity);
					}
				}
			);

			ScriptSystem::saveScripts(data, data.saveDataJson);
			data.currentSceneInitializer->save(data);

			File::writeFile(data.saveDataJson.dump(4).c_str(), filename);
		}

		void load(SceneData& data, const std::filesystem::path& filename, bool setAsCurrentScene)
		{
			Logger::Log("Loading scene %s", filename.string().c_str());
			init(data);

			if (setAsCurrentScene)
			{
				Settings::General::currentScene = filename;
			}

			FileHandle* file = File::openFile(filename);
			if (file->size <= 0)
			{
				File::closeFile(file);
				return;
			}


			json j = json::parse(file->data);
			// TODO: Change this so that the scene doesn't hold the json at all
			data.saveDataJson = j;

			if (j.contains("Assets"))
			{
				AssetManager::loadTexturesFrom(j["Assets"]);
				AssetManager::loadFontsFrom(j["Assets"]);
			}

			deserializeEntities(j, data);

			// After we deserialize the entities, hand it off to the application in case they saved anything as well
			data.currentSceneInitializer->load(data);

			j = {};
			File::closeFile(file);
		}

		void loadScriptsOnly(SceneData& data, const std::filesystem::path& filename)
		{
			FileHandle* file = File::openFile(filename);
			if (file->size <= 0)
			{
				return;
			}

			Logger::Info("Loading scripts only for %s", filename.string().c_str());
			json j = json::parse(file->data);
			int size = !j.contains("Components") ? 0 : j["Components"].size();
			for (int i = 0; i < size; i++)
			{
				json::iterator it = j["Components"][i].begin();
				json component = j["Components"][i];
				if (it.key() != "SpriteRenderer" && it.key() != "Transform" && it.key() != "Rigidbody2D" && it.key() != "Box2D" && it.key() != "AABB" && it.key() != "Tag"
					&& it.key() != "Camera")
				{
					Entity entity = FindOrCreateEntity(component.front()["Entity"], data, data.registry);
					ScriptSystem::deserialize(data, component, entity);
				}
			}

			j = {};
			File::closeFile(file);
		}

		Entity createEntity(SceneData& data)
		{
			entt::entity e = data.registry.create();
			Entity entity = Entity{ e };
			// TODO: Make transform's optional with entities
			TransformData defaultTransform = Transform::createTransform();
			NEntity::addComponent<TransformData>(entity, defaultTransform);
			NEntity::addComponent<Tag>(entity, NTag::createTag("New Entity"));
			return entity;
		}

		Entity duplicateEntity(SceneData& data, Entity entity)
		{
			entt::entity newEntEntity = data.registry.create();
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

		Entity getEntity(SceneData& data, uint32 id)
		{
			entt::entity entity = entt::null;
			if (id < std::numeric_limits<uint32>::max())
			{
				entity = entt::entity(id);
			}
			return Entity{ entity };
		}

		bool isValid(SceneData& scene, uint32 entityId)
		{
			return scene.registry.valid(entt::entity(entityId));
		}

		bool isValid(SceneData& scene, Entity entity)
		{
			return scene.registry.valid(entity.handle);
		}

		void deleteEntity(SceneData& scene, Entity entity)
		{
			// Recursively delete entity and all children
			auto view = scene.registry.view<TransformData>();
			for (entt::entity rawEntity : view)
			{
				Entity potentialChild = NEntity::createEntity(rawEntity);
				TransformData& transformData = NEntity::getComponent<TransformData>(potentialChild);
				if (transformData.parent == entity)
				{
					deleteEntity(scene, potentialChild);
				}
			}

			Physics2D::deleteEntity(entity);
			TransformSystem::deleteEntity(entity);
			CameraSystem::deleteEntity(entity);
			scene.registry.destroy(entity.handle);
		}

		static void LoadDefaultAssets()
		{
			Texture gizmoSpec;
			gizmoSpec.magFilter = FilterMode::Linear;
			gizmoSpec.minFilter = FilterMode::Linear;
			gizmoSpec.wrapS = WrapMode::Repeat;
			gizmoSpec.wrapT = WrapMode::Repeat;
			gizmoSpec.isDefault = true;
			const std::filesystem::path gizmoPath = Settings::General::engineAssetsPath / "images" / "gizmos.png";
			Handle<Texture> asset = AssetManager::loadTextureFromFile(gizmoSpec, gizmoPath);
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