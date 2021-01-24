#pragma once
#include "externalLibs.h"

#include "cocoa/file/CPath.h"
#include "cocoa/systems/System.h"
#include "cocoa/scenes/SceneInitializer.h"

#include <entt/entt.hpp>

namespace Cocoa
{
	class SceneInitializer;
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

		void Play();
		void Stop();
		void Save(const CPath& filename);
		void Load(const CPath& filename);
		void LoadScriptsOnly(const CPath& filename);
		void Reset();

		Entity CreateEntity();
		Entity DuplicateEntity(Entity entity);
		Entity GetEntity(uint32 id);

		inline Camera* GetCamera() { return m_Camera; }
		inline const std::vector<std::unique_ptr<System>>& GetSystems() { return m_Systems; }
		inline entt::registry& GetRegistry() { return m_Registry; }

		// TODO: TEMPORARY GET BETTER SYSTEM THAN THESE!!!
		inline json& GetSaveDataJson() { return m_SaveDataJson; }
		inline void ShowDemoWindow() { m_ShowDemoWindow = true; }
		inline bool IsPlaying() { return m_IsPlaying; }

	protected:
		void LoadDefaultAssets();

	protected:
		bool m_ShowDemoWindow;
		bool m_IsPlaying;
		std::vector<std::unique_ptr<System>> m_Systems;

		entt::registry m_Registry;
		json m_SaveDataJson;

		Camera* m_Camera;
		SceneInitializer* m_SceneInitializer;

		friend class Entity;
	};
}