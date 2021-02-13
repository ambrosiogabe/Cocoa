#pragma once
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "cocoa/util/Log.h"
#include "cocoa/scenes/SceneData.h"

#include <entt/entt.hpp>

namespace Cocoa
{
	struct Entity
	{
		entt::entity Handle;
		SceneData* Scene;
	};

	namespace NEntity
	{
		COCOA Entity CreateEntity(SceneData* scene);
		COCOA Entity CreateEntity(entt::entity raw);
		COCOA void SetScene(SceneData* scene);
		COCOA SceneData* GetScene();
		COCOA Entity CreateNull();

		template<typename... Component>
		bool HasComponent(Entity entity)
		{
			return entity.Scene->Registry.has<Component...>(entity.Handle);
		}

		template<typename T, typename ... Args>
		T& AddComponent(Entity entity, Args&&... args)
		{
			Log::Assert(!HasComponent<T>(entity), "Entity already has component.");

			return entity.Scene->Registry.emplace<T>(entity.Handle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent(Entity entity)
		{
			Log::Assert(HasComponent<T>(entity), "Entity does not have component.");
			return entity.Scene->Registry.get<T>(entity.Handle);
		}

		template<typename T>
		void RemoveComponent(Entity entity)
		{
			Log::Assert(HasComponent<T>(entity), "Entity does not have component.");
			entity.Scene->Registry.remove<T>(entity.Handle);
		}

		inline bool IsNull(Entity entity)
		{
			return entity.Handle == entt::null;
		}

		inline uint32 GetID(Entity entity)
		{
			return (uint32)(entt::to_integral(entity.Handle));
		}

		template<typename T>
		Entity FromComponent(const T& component)
		{
			SceneData* scene = GetScene();
			size_t offset = &component - scene->Registry.raw<T>();
			Log::Assert(offset < scene->Registry.size(), "Tried to get nonexistent entity.");
			return Entity{*(scene->Registry.data<T>() + offset), scene};
		}
	}

	COCOA bool operator==(const Entity& a, const Entity& b);
	COCOA bool operator==(Entity& a, Entity& other);
}