#include "cocoa/scenes/Scene.h"

#include "cocoa/file/OutputArchive.h"
#include "cocoa/file/File.h"
#include "cocoa/util/Settings.h"
#include "cocoa/core/Entity.h"
#include "cocoa/components/Transform.h"
#include "cocoa/components/Tag.h"
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
			NEntity::SetScene(&data);
			Input::SetScene(&data);

			InitComponentIds(data);
			LoadDefaultAssets();

			glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0);
			data.SceneCamera = NCamera::CreateCamera(cameraPos);

			RenderSystem::Init(data);
			Physics2D::Init({ 0, -10.0f });
			ScriptSystem::Init(data);

			data.CurrentSceneInitializer->Init(data);
		}

		static void InitComponentIds(SceneData& scene)
		{
			NEntity::RegisterComponentType<TransformData>();
			NEntity::RegisterComponentType<Tag>();
			NEntity::RegisterComponentType<SpriteRenderer>();
			NEntity::RegisterComponentType<FontRenderer>();
			NEntity::RegisterComponentType<Rigidbody2D>();
			NEntity::RegisterComponentType<Box2D>();
			NEntity::RegisterComponentType<Circle>();
			NEntity::RegisterComponentType<AABB>();
		}

		void Start(SceneData& data)
		{
			data.CurrentSceneInitializer->Start(data);
		}

		void Update(SceneData& data, float dt)
		{
			TransformSystem::Update(data, dt);
			Physics2D::Update(data, dt);
			ScriptSystem::Update(data, dt);
			NCamera::Update(data.SceneCamera);
		}

		void EditorUpdate(SceneData& data, float dt)
		{
			// There are certain systems that use the same update loop for the editor and the actual game, so there's no 
			// sense in creating a unique update loop if the logic is the same (TransformSystem, and NCamera are examples of this)
			TransformSystem::Update(data, dt);
			ScriptSystem::EditorUpdate(data, dt);
			NCamera::Update(data.SceneCamera);
		}

		void OnEvent(SceneData& data, const Event& e)
		{
			// TODO: If you have systems that require events, place them in here
		}

		void Render(SceneData& data)
		{
			NFramebuffer::Bind(RenderSystem::GetMainFramebuffer());

			glEnable(GL_BLEND);
			glViewport(0, 0, 3840, 2160);
			glClearColor(0.45f, 0.55f, 0.6f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			NFramebuffer::ClearColorAttachmentUint32(RenderSystem::GetMainFramebuffer(), 1, (uint32)-1);
			//RenderSystem::UploadUniform1ui("uActiveEntityID", InspectorWindow::GetActiveEntity().GetID() + 1);

			DebugDraw::DrawBottomBatches(data.SceneCamera);
			RenderSystem::Render(data);
			DebugDraw::DrawTopBatches(data.SceneCamera);
		}

		void FreeResources(SceneData& data)
		{
			Log::Log("Freeing scene resources");
			AssetManager::Clear();

			TransformSystem::Destroy(data);
			RenderSystem::Destroy();
			Physics2D::Destroy(data);

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
				Physics2D::AddEntity(Entity{ entity });
			}
		}

		void Stop(SceneData& data)
		{
			data.IsPlaying = false;
		}

		void Save(SceneData& data, const CPath& filename)
		{
			Log::Log("Saving scene '%s'", filename.Path.c_str());
			data.SaveDataJson = {
				{"Components", {}},
				{"Project", Settings::General::s_CurrentProject.Path.c_str()},
				{"Assets", AssetManager::Serialize()},
				{"EditorCamera", NCamera::Serialize(data.SceneCamera)}
			};

			OutputArchive output(data.SaveDataJson);
			entt::snapshot{ data.Registry }
				.entities(output)
				.component<TransformData, Rigidbody2D, Box2D, SpriteRenderer, FontRenderer, AABB, Tag>(output);

			ScriptSystem::SaveScripts(data, data.SaveDataJson);
			data.CurrentSceneInitializer->Save(data);

			File::WriteFile(data.SaveDataJson.dump(4).c_str(), filename);
		}

		void Load(SceneData& data, const CPath& filename, bool setAsCurrentScene)
		{
			Log::Log("Loading scene %s", filename.Path.c_str());
			Init(data);

			if (setAsCurrentScene)
			{
				Settings::General::s_CurrentScene = filename;
			}

			FileHandle* file = File::OpenFile(filename);
			if (file->m_Size <= 0)
			{
				File::CloseFile(file);
				return;
			}


			json j = json::parse(file->m_Data);
			// TODO: Change this so that the scene doesn't hold the json at all
			data.SaveDataJson = j;

			if (j.contains("EditorCamera"))
			{
				NCamera::Deserialize(j["EditorCamera"], data.SceneCamera);
			}

			if (j.contains("Assets"))
			{
				AssetManager::LoadTexturesFrom(j["Assets"]);
				AssetManager::LoadFontsFrom(j["Assets"]);
			}

			int size = !j.contains("Components") ? 0 : j["Components"].size();
			for (int i = 0; i < size; i++)
			{
				json::iterator it = j["Components"][i].begin();
				json component = j["Components"][i];
				if (it.key() == "SpriteRenderer")
				{
					Entity entity = FindOrCreateEntity(component["SpriteRenderer"]["Entity"], data, data.Registry);
					RenderSystem::DeserializeSpriteRenderer(component, entity);
				}
				else if (it.key() == "FontRenderer")
				{
					Entity entity = FindOrCreateEntity(component["FontRenderer"]["Entity"], data, data.Registry);
					RenderSystem::DeserializeFontRenderer(component, entity);
				}
				else if (it.key() == "Transform")
				{
					Entity entity = FindOrCreateEntity(component["Transform"]["Entity"], data, data.Registry);
					Entity parentEntity = FindOrCreateEntity(component["Transform"]["Parent"], data, data.Registry);
					Transform::Deserialize(component, entity, parentEntity);
				}
				else if (it.key() == "Rigidbody2D")
				{
					Entity entity = FindOrCreateEntity(component["Rigidbody2D"]["Entity"], data, data.Registry);
					Physics2D::DeserializeRigidbody2D(component, entity);
				}
				else if (it.key() == "Box2D")
				{
					Entity entity = FindOrCreateEntity(component["Box2D"]["Entity"], data, data.Registry);
					Physics2D::DeserializeBox2D(component, entity);
				}
				else if (it.key() == "AABB")
				{
					Entity entity = FindOrCreateEntity(component["AABB"]["Entity"], data, data.Registry);
					Physics2D::DeserializeAABB(component, entity);
				}
				else if (it.key() == "Tag")
				{
					Entity entity = FindOrCreateEntity(component["Tag"]["Entity"], data, data.Registry);
					NTag::Deserialize(component, entity);
				}
				else
				{
					Entity entity = FindOrCreateEntity(component.front()["Entity"], data, data.Registry);
					ScriptSystem::Deserialize(data, component, entity);
				}
			}

			// After we deserialize the entities, hand it off to the application in case they saved anything as well
			data.CurrentSceneInitializer->Load(data);

			j = {};
			File::CloseFile(file);
		}

		void LoadScriptsOnly(SceneData& data, const CPath& filename)
		{
			FileHandle* file = File::OpenFile(filename);
			if (file->m_Size <= 0)
			{
				return;
			}

			Log::Info("Loading scripts only for %s", filename.Path.c_str());
			json j = json::parse(file->m_Data);
			int size = !j.contains("Components") ? 0 : j["Components"].size();
			for (int i = 0; i < size; i++)
			{
				json::iterator it = j["Components"][i].begin();
				json component = j["Components"][i];
				if (it.key() != "SpriteRenderer" && it.key() != "Transform" && it.key() != "Rigidbody2D" && it.key() != "Box2D" && it.key() != "AABB" && it.key() != "Tag")
				{
					Entity entity = FindOrCreateEntity(component.front()["Entity"], data, data.Registry);
					ScriptSystem::Deserialize(data, component, entity);
				}
			}

			j = {};
			File::CloseFile(file);
		}

		Entity CreateEntity(SceneData& data)
		{
			entt::entity e = data.Registry.create();
			Entity entity = Entity{ e };
			TransformData defaultTransform = Transform::CreateTransform();
			NEntity::AddComponent<TransformData>(entity, defaultTransform);
			NEntity::AddComponent<Tag>(entity, NTag::CreateTag("New Entity"));
			return entity;
		}

		Entity DuplicateEntity(SceneData& data, Entity entity)
		{
			entt::entity newEntEntity = data.Registry.create();
			Entity newEntity = Entity{ newEntEntity };
			if (NEntity::HasComponent<TransformData>(entity))
			{
				NEntity::AddComponent<TransformData>(newEntity, NEntity::GetComponent<TransformData>(entity));
			}

			if (NEntity::HasComponent<SpriteRenderer>(entity))
			{
				NEntity::AddComponent<SpriteRenderer>(newEntity, NEntity::GetComponent<SpriteRenderer>(entity));
			}

			if (NEntity::HasComponent<Rigidbody2D>(entity))
			{
				NEntity::AddComponent<Rigidbody2D>(newEntity, NEntity::GetComponent<Rigidbody2D>(entity));
			}

			if (NEntity::HasComponent<Box2D>(entity))
			{
				NEntity::AddComponent<Box2D>(newEntity, NEntity::GetComponent<Box2D>(entity));
			}

			if (NEntity::HasComponent<Tag>(entity))
			{
				Tag& tag = NEntity::GetComponent<Tag>(entity);
				char* newTagName = (char*)AllocMem((tag.Size + 1) * sizeof(char));
				memcpy(newTagName, tag.Name, (tag.Size + 1) * sizeof(char));
				NEntity::AddComponent<Tag>(newEntity, NTag::CreateTag(newTagName, true));
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
			return scene.Registry.valid(entity.Handle);
		}

		void DeleteEntity(SceneData& scene, Entity entity)
		{
			// Recursively delete entity and all children
			auto view = scene.Registry.view<TransformData>();
			for (entt::entity rawEntity : view)
			{
				Entity potentialChild = NEntity::CreateEntity(rawEntity);
				TransformData& transformData = NEntity::GetComponent<TransformData>(potentialChild);
				if (transformData.Parent == entity)
				{
					DeleteEntity(scene, potentialChild);
				}
			}

			Physics2D::DeleteEntity(entity);
			TransformSystem::DeleteEntity(entity);
			scene.Registry.destroy(entity.Handle);
		}

		static void LoadDefaultAssets()
		{
			Texture gizmoSpec;
			gizmoSpec.MagFilter = FilterMode::Linear;
			gizmoSpec.MinFilter = FilterMode::Linear;
			gizmoSpec.WrapS = WrapMode::Repeat;
			gizmoSpec.WrapT = WrapMode::Repeat;
			gizmoSpec.IsDefault = true;
			CPath gizmoPath = Settings::General::s_EngineAssetsPath;
			NCPath::Join(gizmoPath, NCPath::CreatePath("images/gizmos.png"));
			auto asset = AssetManager::LoadTextureFromFile(gizmoSpec, gizmoPath);
		}

		static Entity FindOrCreateEntity(int id, SceneData& scene, entt::registry& registry)
		{
			Entity entity;
			if (entt::entity(id) == entt::null)
			{
				return NEntity::CreateNull();
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