#pragma once
#include "externalLibs.h"
#include "cocoa/core/Entity.h"

namespace Cocoa
{
	struct TransformData
	{
		glm::vec3 Position;
		glm::vec3 Scale;
		glm::vec3 EulerRotation;
		glm::quat Orientation;

		glm::vec3 Forward;
		glm::vec3 Up;
		glm::vec3 Right;

		glm::mat4 ModelMatrix;
		glm::mat4 InverseModelMatrix;
	};

	namespace Transform
	{
		COCOA TransformData CreateTransform();
		COCOA TransformData CreateTransform(glm::vec3 position, glm::vec3 scale, glm::vec3 eulerRotation);

		COCOA void UpdateMatrices(TransformData& data);

		COCOA void Serialize(json& j, Entity entity, const TransformData& transform);
		COCOA void Deserialize(json& j, Entity entity);
	}
}