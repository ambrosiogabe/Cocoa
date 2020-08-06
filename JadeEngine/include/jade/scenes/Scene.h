#pragma once
#include "externalLibs.h"

#include "jade/file/JPath.h"
#include "jade/systems/System.h"
#include "jade/core/AssetManager.h"

#include <entt/entt.hpp>

namespace Jade
{
	class Entity;

	class Scene
	{
	public:
		virtual void Init() = 0;
		virtual void Render() = 0;
		virtual void ImGui() = 0;
		virtual void Update(float dt) = 0;
		virtual void Start() = 0;

		void Play();
		void Stop();
		void Save(const JPath& filename);
		void Load(const JPath& filename);
		void Reset();

		Entity CreateEntity();
		Entity GetEntity(uint32 id);

		inline Camera* GetCamera() { return m_Camera; }
		inline const std::vector<std::unique_ptr<System>>& GetSystems() { return m_Systems; }
		inline entt::registry& GetRegistry() { return m_Registry; }

		// TODO: TEMPORARY GET BETTER SYSTEM THAN THESE!!!
		inline json& GetSaveDataJson() { return m_SaveDataJson; }
		inline void ShowDemoWindow() { m_ShowDemoWindow = true; }

	protected:
		void LoadDefaultAssets();

	protected:
		bool m_ShowDemoWindow = false;
		bool m_IsRunning = false;
		std::vector<std::unique_ptr<System>> m_Systems;

		entt::registry m_Registry = entt::registry();
		json m_SaveDataJson;

		Camera* m_Camera;

		friend class Entity;
	};
}