#ifndef COCOA_ENGINE_ENTITY_H
#define COCOA_ENGINE_ENTITY_H
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "cocoa/scenes/SceneData.h"
#include "cocoa/core/EntityStruct.h"

namespace Cocoa
{
	namespace NEntity
	{
		COCOA Entity createEntity(entt::entity raw);
		COCOA void setScene(SceneData* scene);
		COCOA SceneData* getScene();
		COCOA Entity createNull();

		template<typename Component>
		void registerComponentType()
		{
			SceneData* scene = getScene();
			scene->Registry.prepare<Component>();
		}

		template<typename Type>
		void clear()
		{
			SceneData* scene = getScene();
			scene->Registry.clear<Type>();
		}

		template<typename... Component>
		bool hasComponent(Entity entity)
		{
			SceneData* scene = getScene();
			return scene->Registry.has<Component...>(entity.handle);
		}

		template<typename T, typename ... Args>
		T& addComponent(Entity entity, Args&&... args)
		{
			Logger::Assert(!hasComponent<T>(entity), "Entity already has component.");
			SceneData* scene = getScene();
			return scene->Registry.emplace<T>(entity.handle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& getComponent(Entity entity)
		{
			Logger::Assert(hasComponent<T>(entity), "Entity does not have component.");
			SceneData* scene = getScene();
			return scene->Registry.get<T>(entity.handle);
		}

		template<typename T>
		void removeComponent(Entity entity)
		{
			Logger::Assert(hasComponent<T>(entity), "Entity does not have component.");
			SceneData* scene = getScene();
			scene->Registry.remove<T>(entity.handle);
		}

		inline bool isNull(Entity entity)
		{
			return entity.handle == entt::null;
		}

		inline uint32 getId(Entity entity)
		{
			return (uint32)(entt::to_integral(entity.handle));
		}

		template<typename T>
		Entity fromComponent(const T& component)
		{
			SceneData* scene = getScene();
			size_t offset = &component - scene->Registry.raw<T>();
			Logger::Assert(offset < scene->Registry.size(), "Tried to get nonexistent entity.");
			return Entity{*(scene->Registry.data<T>() + offset)};
		}
	}

	COCOA bool operator==(const Entity& a, const Entity& b);
	COCOA bool operator==(Entity& a, Entity& other);
	COCOA bool operator!=(const Entity& a, const Entity& b);
	COCOA bool operator!=(Entity& a, Entity& b);
}

#endif
