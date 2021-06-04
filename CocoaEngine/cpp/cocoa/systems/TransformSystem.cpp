#include "cocoa/systems/TransformSystem.h"
#include "cocoa/components/Transform.h"
#include "cocoa/components/Tag.h"

namespace Cocoa
{
	namespace TransformSystem
	{
		void update(SceneData& scene, float dt)
		{
			auto view = scene.Registry.view<TransformData>();
			for (entt::entity rawEntity : view)
			{
				Entity entity = NEntity::createEntity(rawEntity);
				TransformData& transform = NEntity::getComponent<TransformData>(entity);
				Transform::update(transform, dt);
			}
		}

		void Destroy(SceneData& scene)
		{
			auto view = scene.Registry.view<Tag>();
			for (entt::entity rawEntity : view)
			{
				Entity entity = NEntity::createEntity(rawEntity);
				Tag& tag = NEntity::getComponent<Tag>(entity);
				NTag::destroy(tag);
			}
		}

		void DeleteEntity(Entity entity)
		{
			if (NEntity::hasComponent<Tag>(entity))
			{
				Tag& tag = NEntity::getComponent<Tag>(entity);
				NTag::destroy(tag);
			}
		}
	}
}