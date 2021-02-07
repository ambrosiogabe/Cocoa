#pragma once
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "cocoa/file/CPath.h"
#include "cocoa/scenes/SceneInitializer.h"
#include "cocoa/renderer/PickingTexture.h"
#include "cocoa/renderer/Shader.h"
#include "cocoa/core/Handle.h"
#include "cocoa/events/Event.h"

#include <entt/entt.hpp>

namespace Cocoa
{
	class Camera;
	class Entity;
	class COCOA Scene
	{
	public:
		Scene(SceneInitializer* sceneInitializer);

		void Init();
		void Start();
		void Update(float dt);
		void EditorUpdate(float dt);
		void Render();
		void Destroy();

		void Play();
		void Stop();
		void Save(const CPath& filename);
		void Load(const CPath& filename);
		void LoadScriptsOnly(const CPath& filename);
		void Reset();
		void OnEvent(const Event& e);

		Entity CreateEntity();
		Entity DuplicateEntity(Entity entity);
		Entity GetEntity(uint32 id);

		inline Camera* GetCamera() { return m_Camera; }
		inline entt::registry& GetRegistry() { return m_Registry; }

		// TODO: TEMPORARY GET BETTER SYSTEM THAN THESE!!!
		inline json& GetSaveDataJson() { return m_SaveDataJson; }
		inline bool IsPlaying() { return m_IsPlaying; }

	public:
		PickingTexture m_PickingTexture;

	protected:
		void LoadDefaultAssets();

	protected:
		Handle<Shader> m_PickingShader;
		Handle<Shader> m_DefaultShader;

		bool m_IsPlaying;

		entt::registry m_Registry;
		json m_SaveDataJson;

		Camera* m_Camera;
		SceneInitializer* m_SceneInitializer;

		friend class Entity;
	};
}