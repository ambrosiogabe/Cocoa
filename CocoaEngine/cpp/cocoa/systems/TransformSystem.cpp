#include "cocoa/systems/TransformSystem.h"
#include "cocoa/components/Transform.h"

namespace Cocoa
{
	namespace TransformSystem
	{
		void Update(SceneData& data, float dt)
		{
			auto view = data.Registry.view<TransformData>();
			for (entt::entity rawEntity : view)
			{
				Entity entity = NEntity::CreateEntity(rawEntity);
				TransformData& transform = NEntity::GetComponent<TransformData>(entity);
				Transform::Update(transform, dt);
			}
		}
	}
}