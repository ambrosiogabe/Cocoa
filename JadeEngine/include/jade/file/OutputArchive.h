#pragma once
#include "externalLibs.h"

#include "jade/systems/RenderSystem.h"
#include "jade/physics2d/Physics2DSystem.h"
#include "jade/util/Log.h"

#include <nlohmann/json.hpp>

namespace Jade
{
	class OutputArchive
	{
	public:
		OutputArchive(json& j) 
			: m_Json(j) {}

		void operator()(entt::entity entity)
		{
			//Log::Info("Archiving entity: %d", entt::to_integral(entity));
		}

		void operator()(std::underlying_type_t<entt::entity> underlyingType)
		{
			//Log::Info("Archiving underlying type entity: %d", underlyingType);
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
					Transform::Serialize(m_Json, entity, *transform);
					break;
				}
				case entt::type_info<SpriteRenderer>().id():
				{
					const SpriteRenderer* renderer = reinterpret_cast<const SpriteRenderer*>(&component);
					RenderSystem::Serialize(m_Json, entity, *renderer);
					break;
				}
				case entt::type_info<Box2D>().id():
				{
					const Box2D* box2D = reinterpret_cast<const Box2D*>(&component);
					Physics2DSystem::Serialize(m_Json, entity, *box2D);
					break;
				}
				case entt::type_info<Rigidbody2D>().id():
				{
					const Rigidbody2D* rb = reinterpret_cast<const Rigidbody2D*>(&component);
					Physics2DSystem::Serialize(m_Json, entity, *rb);
					break;
				}
				case entt::type_info<AABB>().id():
				{
					const AABB* box = reinterpret_cast<const AABB*>(&component);
					Physics2DSystem::Serialize(m_Json, entity, *box);
					break;
				}
				default: 
				{
					//Log::Info("Archiving entity: %d  DEFAULT", entt::to_integral(entity));
					break;
				}
			}
		}

	private:
		json& m_Json;
	};
}