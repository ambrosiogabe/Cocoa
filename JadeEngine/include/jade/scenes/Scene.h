#pragma once
#include "externalLibs.h"

#include "jade/file/JPath.h"
#include "jade/systems/RenderSystem.h"

namespace Jade
{
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

		inline void SetActiveAsset(std::shared_ptr<Asset> asset)
		{
			m_ActiveAsset = asset;
			m_ActiveEntity = entt::null;
		}

		inline void SetActiveEntity(entt::entity entity)
		{
			m_ActiveEntity = entity;
			m_ActiveAsset = nullptr;
		}

		inline entt::entity GetActiveEntity() { return m_ActiveEntity; }
		inline std::shared_ptr<Asset> GetActiveAsset() { return m_ActiveAsset; }
		inline Camera* GetCamera() { return m_Camera; }
		inline entt::registry& GetRegistry() { return m_Registry; }
		inline const std::vector<std::unique_ptr<System>>& GetSystems() { return m_Systems; }


		// TODO: TEMPORARY GET BETTER SYSTEM THAN THESE!!!
		inline json& GetSaveDataJson() { return m_SaveDataJson; }
		inline void ShowDemoWindow() { m_ShowDemoWindow = true; }

	public:
		template<typename T>
		std::pair<entt::registry&, entt::entity> GetEntity(const T& component)
		{
			size_t offset = &component - m_Registry.raw<T>();
			Log::Assert(offset < m_Registry.size(), "Tried to get nonexistent entity.");
			return { m_Registry, *(m_Registry.data<T>() + offset) };
		}

	protected:
		void LoadDefaultAssets();

	protected:
		bool m_ShowDemoWindow = false;
		bool m_IsRunning = false;
		std::vector<std::unique_ptr<System>> m_Systems;

		entt::registry m_Registry = entt::registry();
		json m_SaveDataJson;

		Camera* m_Camera;
		entt::entity m_ActiveEntity = entt::null;
		std::shared_ptr<Asset> m_ActiveAsset = nullptr;
	};
}