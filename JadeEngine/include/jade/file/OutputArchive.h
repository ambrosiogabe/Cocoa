#pragma once
#include "externalLibs.h"

#include "jade/systems/RenderSystem.h"
#include "jade/util/Log.h"

namespace Jade
{
	class OutputArchive
	{
	public:
		void operator()(entt::entity entity)
		{
			Log::Info("Archiving entity: %d", entt::to_integral(entity));
		}

		void operator()(std::underlying_type_t<entt::entity> underlyingType)
		{
			Log::Info("Archiving underlying type entity: %d", underlyingType);
		}

		template <class T>
		void operator()(entt::entity entity, const T& component)
		{
			entt::type_info info = entt::type_info<T>();
			switch (info.id())
			{
				case entt::type_info<Transform>().id():
				{
					const Transform* transform = reinterpret_cast<const Transform*>(&component);
					Transform::Serialize(entity, *transform);
					break;
				}
				case entt::type_info<SpriteRenderer>().id():
				{
					const SpriteRenderer* renderer = reinterpret_cast<const SpriteRenderer*>(&component);
					RenderSystem::Serialize(entity, *renderer);
					break;
				}
				case entt::type_info<Box2D>().id():
				{
					Log::Info("Archiving entity: %d  Box2D", entt::to_integral(entity));
					break;
				}
				case entt::type_info<Rigidbody2D>().id():
				{
					Log::Info("Archiving entity: %d  Rigidbody2D", entt::to_integral(entity));
					break;
				}
				default: 
				{
					Log::Info("Archiving entity: %d  DEFAULT", entt::to_integral(entity));
					break;
				}
			}
		}
	};
}