#pragma once
#include "externalLibs.h"
#include "cocoa/util/Log.h"
#include "cocoa/scenes/Scene.h"

#include <entt/entt.hpp>

namespace Cocoa
{
	class Scene;
	class COCOA Entity
	{
	public:
		Entity();
		Entity(entt::entity handle, Scene* scene);
		Entity(entt::entity& other);
		Entity(const entt::entity& other);
		Entity(const Entity& other) = default;

		template<typename T, typename ... Args>
		T& AddComponent(Args&&... args)
		{
			Log::Assert(!HasComponent<T>(), "Entity already has component.");

			return s_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent()
		{
			Log::Assert(HasComponent<T>(), "Entity does not have component.");
			return s_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		const T& GetComponent() const
		{
			Log::Assert(HasComponent<T>(), "Entity does not have component.");
			return s_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename... Component>
		bool HasComponent() const
		{
			return s_Scene->m_Registry.has<Component...>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			Log::Assert(HasComponent<T>(), "Entity does not have component.");
			s_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		inline bool IsNull() const
		{
			return m_EntityHandle == entt::null;
		}

		inline uint32 GetID() const
		{
			return (uint32)(entt::to_integral(m_EntityHandle));
		}

		template<typename T>
		static Entity FromComponent(const T& component)
		{
			size_t offset = &component - s_Scene->m_Registry.raw<T>();
			Log::Assert(offset < s_Scene->m_Registry.size(), "Tried to get nonexistent entity.");
			return Entity(*(s_Scene->m_Registry.data<T>() + offset), s_Scene); // { m_Registry, * (m_Registry.data<T>() + offset) };
		}

		entt::entity GetRawEntity()
		{
			return m_EntityHandle;
		}

		entt::registry& GetRegistry()
		{
			return s_Scene->GetRegistry();
		}

		bool operator==(const Entity& other) const;
		bool operator==(Entity& other) const;

		static void SetScene(Scene* currentScene);

	public:
		static Entity Null;

	private:
		static Scene* s_Scene;
		entt::entity m_EntityHandle;
	};
}