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
		COCOA Camera CreateCamera(Framebuffer framebuffer);

		COCOA void AdjustPerspective(Camera& camera);
		COCOA void CalculateViewMatrix(Entity entity, Camera& camera);
		COCOA void CalculateOrthoViewMatrix(Entity entity, Camera& camera);

		COCOA void ClearColorRgb(const Camera& camera, int attachment, glm::vec3 color);
		COCOA void ClearColorUint32(Camera& camera, int attachment, uint32 color);

		COCOA glm::vec2 ScreenToOrtho(const Camera& camera);

		COCOA void Serialize(json* j, Entity entity, const Camera& camera);
		COCOA void Deserialize(const json& j, Entity entity);
	};

	namespace CameraSystem
	{
		COCOA void Update(SceneData& scene, float dt);
		COCOA void Destroy(SceneData& scene);
		COCOA void DeleteEntity(Entity entity);
	}
}