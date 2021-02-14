#include "cocoa/scenes/Scene.h"

#include "cocoa/file/OutputArchive.h"
#include "cocoa/file/IFile.h"
#include "cocoa/util/Settings.h"
#include "cocoa/core/Entity.h"
#include "cocoa/components/Transform.h"
#include "cocoa/systems/ScriptSystem.h"
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

			data.SceneCamera = nullptr;
			data.IsPlaying = false;

			data.Registry = entt::registry();
			return data;
		}

		void Init(SceneData& data)
		{
			LoadDefaultAssets();

			glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0);
			data.SceneCamera = new Camera(cameraPos);

			Input::SetScene(&data);
			NEntity::SetScene(&data);

			RenderSystem::Init(data);
			Physics2DSystem::Init({ 0, -10.0f });

			data.CurrentSceneInitializer->Init(data);
		}

		void Start(SceneData& data)
		{
			ScriptSystem::Start();
			data.CurrentSceneInitializer->Start(data);
		}

		void Update(SceneData& data, float dt)
		{
			Physics2DSystem::Update(data, dt);
			ScriptSystem::Update(data, dt);
		}

		void EditorUpdate(SceneData& data, float dt)
		{
			ScriptSystem::EditorUpdate(data, dt);
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

			DebugDraw::DrawBottomBatches();
			RenderSystem::Render(data);
			DebugDraw::DrawTopBatches();
		}

		void Destroy(SceneData& data)
		{
			AssetManager::Clear();
			auto view = data.Registry.view<TransformData>();
			data.Registry.destroy(view.begin(), view.end());

			RenderSystem::Destroy();
			Physics2DSystem::Destroy(data);
			ScriptSystem::FreeScriptLibrary();

			delete data.SceneCamera;
		}

		void Play(SceneData& data)
		{
			data.IsPlaying = true;
			auto view = data.Registry.view<TransformData>();
			for (auto entity : view)
			{
				Physics2DSystem::AddEntity(Entity{entity, &data});
			}
		}

		void Stop(SceneData& data)
		{
			data.IsPlaying = false;
		}

		void Save(SceneData& data, const CPath& filename)
		{
			Log::Info("Saving scene for %s", filename.Path.c_str());
			data.SaveDataJson = {
				{"Components", {}},
				{"Project", Settings::General::s_CurrentProject.Path.c_str()},
				{"Assets", AssetManager::Serialize()}
			};

			OutputArchive output(data.SaveDataJson);
			entt::snapshot{ data.Registry }
				.entities(output)
				.component<TransformData, Rigidbody2D, Box2D, SpriteRenderer, FontRenderer, AABB>(output);

			ScriptSystem::SaveScripts(data.SaveDataJson);

			IFile::WriteFile(data.SaveDataJson.dump(4).c_str(), filename);
		}

		void Load(SceneData& data, const CPath& filename)
		{
			Reset(data);
			Log::Info("Loading scene %s", filename.Path.c_str());

			Settings::General::s_CurrentScene = filename;
			File* file = IFile::OpenFile(filename);
			if (file->m_Data.size() <= 0)
			{
				return;
			}

			Start(data);

			json j = json::parse(file->m_Data);

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
					Transform::Deserialize(component, entity);
				}
				else if (it.key() == "Rigidbody2D")
				{
					Entity entity = FindOrCreateEntity(component["Rigidbody2D"]["Entity"], data, data.Registry);
					Physics2DSystem::DeserializeRigidbody2D(component, entity);
				}
				else if (it.key() == "Box2D")
				{
					Entity entity = FindOrCreateEntity(component["Box2D"]["Entity"], data, data.Registry);
					Physics2DSystem::DeserializeBox2D(component, entity);
				}
				else if (it.key() == "AABB")
				{
					Entity entity = FindOrCreateEntity(component["AABB"]["Entity"], data, data.Registry);
					Physics2DSystem::DeserializeAABB(component, entity);
				}
				else
				{
					Entity entity = FindOrCreateEntity(component.front()["Entity"], data, data.Registry);
					ScriptSystem::Deserialize(component, entity);
				}
			}

			IFile::CloseFile(file);
		}

		void LoadScriptsOnly(SceneData& data, const CPath& filename)
		{
			File* file = IFile::OpenFile(filename);
			if (file->m_Data.size() <= 0)
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
				if (it.key() != "SpriteRenderer" && it.key() != "Transform" && it.key() != "Rigidbody2D" && it.key() != "Box2D" && it.key() != "AABB")
				{
					Entity entity = FindOrCreateEntity(component.front()["Entity"], data, data.Registry);
					ScriptSystem::Deserialize(component, entity);
				}
			}

			IFile::CloseFile(file);
		}

		void Reset(SceneData& data)
		{
			Destroy(data);
			Init(data);
		}

		Entity CreateEntity(SceneData& data)
		{
			entt::entity e = data.Registry.create();
			Entity entity = Entity{e, &data};
			NEntity::AddComponent<TransformData>(entity);
			return entity;
		}

		Entity DuplicateEntity(SceneData& data, Entity entity)
		{
			entt::entity newEntEntity = data.Registry.create();
			Entity newEntity = Entity{newEntEntity, &data};
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

			return newEntity;
		}

		Entity Scene::GetEntity(SceneData& data, uint32 id)
		{
			entt::entity entity = entt::null;
			if (id < std::numeric_limits<uint32>::max())
			{
				entity = entt::entity(id);
			}
			return Entity{entity, &data};
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
			if (registry.valid(entt::entity(id)))
			{
				entity = Entity{entt::entity(id), &scene};
			}
			else
			{
				entity = Entity{registry.create(entt::entity(id)), &scene};
			}

			return entity;
		}
	}
}