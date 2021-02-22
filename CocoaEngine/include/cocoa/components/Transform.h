#pragma once
#include "externalLibs.h"
#include "cocoa/core/Entity.h"
#include "cocoa/components/TransformStruct.h"

namespace Cocoa
{
	namespace Transform
	{
		COCOA TransformData CreateTransform();
		COCOA TransformData CreateTransform(glm::vec3 position, glm::vec3 scale, glm::vec3 eulerRotation);

		COCOA void Update(TransformData& data, float dt);

		COCOA void Serialize(json& j, Entity entity, const TransformData& transform);
		COCOA void Deserialize(json& j, Entity entity);
	}

	namespace TransformSystem
	{
		COCOA void Update(SceneData& scene, float dt);
	}
}