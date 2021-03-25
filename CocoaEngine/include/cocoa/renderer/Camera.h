#pragma once
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "cocoa/components/Transform.h"
#include "cocoa/util/Log.h"
#include "cocoa/renderer/CameraStruct.h"
#include "cocoa/renderer/Framebuffer.h"

namespace Cocoa
{
	namespace NCamera
	{
		COCOA Camera CreateCamera();
		COCOA Camera CreateCamera(glm::vec3& position, Framebuffer framebuffer);

		COCOA void AdjustPerspective(Camera& camera);
		COCOA void CalculateViewMatrix(Camera& camera);
		COCOA void CalculateOrthoViewMatrix(Camera& camera);

		COCOA glm::vec2 ScreenToOrtho(const Camera& camera);

		COCOA json Serialize(const Camera& camera);
		COCOA void Deserialize(const json& j, Camera& camera);
	};

	namespace CameraSystem
	{
		COCOA void Update(SceneData& scene, float dt);
		COCOA void Destroy(SceneData& scene);
	}
}