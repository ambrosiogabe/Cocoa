#pragma once
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "cocoa/components/Transform.h"
#include "cocoa/util/Log.h"
#include "cocoa/renderer/CameraStruct.h"

namespace Cocoa
{
	namespace NCamera
	{
		COCOA Camera CreateCamera(glm::vec3& position);

		COCOA void Update(Camera& camera);
		COCOA void AdjustPerspective(Camera& camera);
		COCOA void CalculateViewMatrix(Camera& camera);
		COCOA void CalculateOrthoViewMatrix(Camera& camera);

		COCOA glm::vec2 ScreenToOrtho(const Camera& camera);
	};
}