#ifndef COCOA_ENGINE_TRANSFORM_STRUCT_H
#define COCOA_ENGINE_TRANSFORM_STRUCT_H
#include "externalLibs.h"
#include "cocoa/core/EntityStruct.h"

namespace Cocoa
{
	struct TransformData
	{
		glm::vec3 position;
		glm::vec3 scale;
		glm::vec3 eulerRotation;
		glm::quat orientation;

		glm::mat4 modelMatrix;
		glm::mat4 inverseModelMatrix;
		glm::vec3 localPosition;
		glm::vec3 localScale;
		glm::vec3 localEulerRotation;
		Entity parent;
	};
}

#endif
