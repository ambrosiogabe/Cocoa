#ifndef COCOA_ENGINE_OUTPUT_ARCHIVE_H
#define COCOA_ENGINE_OUTPUT_ARCHIVE_H
#include "externalLibs.h"

#include "cocoa/components/Transform.h"
#include "cocoa/components/Tag.h"
#include "cocoa/systems/RenderSystem.h"
#include "cocoa/physics2d/Physics2D.h"

#include <nlohmann/json.hpp>
#include <entt/core/type_info.hpp>

namespace Cocoa
{
	class COCOA OutputArchive
	{
	public:
		OutputArchive(json& j) 
			: mJson(j) {}

		void operator()(entt::entity entity) const
		{
			//Logger::Info("Archiving entity: %d", entt::to_integral(entity));
		}

		void operator()(std::underlying_type_t<entt::entity> underlyingType) const
		{
			//Logger::Info("Archiving underlying type entity: %d", underlyingType);
		}

		template <class T>
		void operator()(entt::entity entity, const T& component)
		{
			entt::type_info info = entt::type_id<T>();

			if (info.seq() == entt::type_id<TransformData>().seq())
			{
				const TransformData* transform = reinterpret_cast<const TransformData*>(&component);
				Transform::serialize(mJson, NEntity::createEntity(entity), *transform);
			} 
			else if (info.seq() == entt::type_id<SpriteRenderer>().seq())
			{
				const SpriteRenderer* renderer = reinterpret_cast<const SpriteRenderer*>(&component);
				RenderSystem::Serialize(mJson, NEntity::createEntity(entity), *renderer);
			}
			else if (info.seq() == entt::type_id<FontRenderer>().seq())
			{
				const FontRenderer* fontRenderer = reinterpret_cast<const FontRenderer*>(&component);
				RenderSystem::Serialize(mJson, NEntity::createEntity(entity), *fontRenderer);
			}
			else if (info.seq() == entt::type_id<Box2D>().seq())
			{
				const Box2D* box2D = reinterpret_cast<const Box2D*>(&component);
				Physics2D::serialize(mJson, NEntity::createEntity(entity), *box2D);
			}
			else if (info.seq() == entt::type_id<Rigidbody2D>().seq())
			{
				const Rigidbody2D* rb = reinterpret_cast<const Rigidbody2D*>(&component);
				Physics2D::serialize(mJson, NEntity::createEntity(entity), *rb);
			}
			else if (info.seq() == entt::type_id<AABB>().seq())
			{
				const AABB* box = reinterpret_cast<const AABB*>(&component);
				Physics2D::serialize(mJson, NEntity::createEntity(entity), *box);
			}
			else if (info.seq() == entt::type_id<Tag>().seq())
			{
				const Tag* tag = reinterpret_cast<const Tag*>(&component);
				NTag::serialize(mJson, NEntity::createEntity(entity), *tag);
			}
			else
			{
				//Logger::Info("Archiving entity: %d  DEFAULT", entt::to_integral(entity));
			}
		}

	private:
		json& mJson;
	};
}

#endif
