#ifndef COCOA_ENGINE_TRANSFORM_H
#define COCOA_ENGINE_TRANSFORM_H
#include "externalLibs.h"
#include "cocoa/core/Entity.h"
#include "cocoa/components/TransformStruct.h"

namespace Cocoa
{
	namespace Transform
	{
		COCOA TransformData createTransform();
		COCOA TransformData createTransform(glm::vec3 position, glm::vec3 scale, glm::vec3 eulerRotation);

		COCOA void update(TransformData& data, float dt);

		COCOA void serialize(json& j, Entity entity, const TransformData& transform);
		COCOA void deserialize(const json& j, Entity entity, Entity parent = NEntity::createNull());
		COCOA void serialize(json& j, const TransformData& transform);
		COCOA void deserialize(const json& j, TransformData& transform);
	}

	namespace TransformSystem
	{
		COCOA void update(SceneData& scene, float dt);
	}
}

#endif
