#pragma once
#include "externalLibs.h"

namespace Cocoa
{
	struct TransformData
	{
		glm::vec3 Position;
		glm::vec3 Scale;
		glm::vec3 EulerRotation;
		glm::quat Orientation;

		glm::mat4 ModelMatrix;
		glm::mat4 InverseModelMatrix;
	};
}