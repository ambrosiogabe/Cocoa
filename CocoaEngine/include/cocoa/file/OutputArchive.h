#pragma once
#include "externalLibs.h"

#include "cocoa/components/Transform.h"
#include "cocoa/systems/RenderSystem.h"
#include "cocoa/physics2d/Physics2D.h"
#include "cocoa/util/Log.h"

#include <nlohmann/json.hpp>

namespace Cocoa
{
	class COCOA OutputArchive
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

			if (info.id() == entt::type_info<TransformData>().id())
			{
				const TransformData* transform = reinterpret_cast<const TransformData*>(&component);
				Transform::Serialize(m_Json, NEntity::CreateEntity(entity), *transform);
			} 
			else if (info.id() == entt::type_info<SpriteRenderer>().id())
			{
				const SpriteRenderer* renderer = reinterpret_cast<const SpriteRenderer*>(&component);
				RenderSystem::Serialize(m_Json, NEntity::CreateEntity(entity), *renderer);
			}
			else if (info.id() == entt::type_info<FontRenderer>().id())
			{
				const FontRenderer* fontRenderer = reinterpret_cast<const FontRenderer*>(&component);
				RenderSystem::Serialize(m_Json, NEntity::CreateEntity(entity), *fontRenderer);
			}
			else if (info.id() == entt::type_info<Box2D>().id())
			{
				const Box2D* box2D = reinterpret_cast<const Box2D*>(&component);
				Physics2D::Serialize(m_Json, NEntity::CreateEntity(entity), *box2D);
			}
			else if (info.id() == entt::type_info<Rigidbody2D>().id())
			{
				const Rigidbody2D* rb = reinterpret_cast<const Rigidbody2D*>(&component);
				Physics2D::Serialize(m_Json, NEntity::CreateEntity(entity), *rb);
			}
			else if (info.id() == entt::type_info<AABB>().id())
			{
				const AABB* box = reinterpret_cast<const AABB*>(&component);
				Physics2D::Serialize(m_Json, NEntity::CreateEntity(entity), *box);
			}
			else
			{
				//Log::Info("Archiving entity: %d  DEFAULT", entt::to_integral(entity));
			}
		}

	private:
		json& m_Json;
	};
}