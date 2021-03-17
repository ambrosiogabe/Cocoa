#include "cocoa/systems/TransformSystem.h"
#include "cocoa/components/Transform.h"
#include "cocoa/components/Tag.h"

namespace Cocoa
{
	namespace TransformSystem
	{
		void Update(SceneData& scene, float dt)
		{
			auto view = scene.Registry.view<TransformData>();
			for (entt::entity rawEntity : view)
			{
				Entity entity = NEntity::CreateEntity(rawEntity);
				TransformData& transform = NEntity::GetComponent<TransformData>(entity);
				Transform::Update(transform, dt);
			}
		}

		void Destroy(SceneData& scene)
		{
			auto view = scene.Registry.view<Tag>();
			for (entt::entity rawEntity : view)
			{
				Entity entity = NEntity::CreateEntity(rawEntity);
				Tag& tag = NEntity::GetComponent<Tag>(entity);
				NTag::Destroy(tag);
			}
		}

		void DeleteEntity(Entity entity)
		{
			if (NEntity::HasComponent<Tag>(entity))
			{
				Tag& tag = NEntity::GetComponent<Tag>(entity);
				NTag::Destroy(tag);
			}
		}
	}
}