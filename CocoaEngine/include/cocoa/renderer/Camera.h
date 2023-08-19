#ifndef COCOA_ENGINE_CAMERA_H
#define COCOA_ENGINE_CAMERA_H
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "cocoa/components/Transform.h"
#include "cocoa/renderer/CameraStruct.h"
#include "cocoa/renderer/Framebuffer.h"

namespace Cocoa
{
	namespace NCamera
	{
		COCOA Camera createCamera();
		COCOA Camera createCamera(Framebuffer framebuffer);

		COCOA void adjustPerspective(Camera& camera);
		COCOA void calculateViewMatrix(Entity entity, Camera& camera);
		COCOA void calculateOrthoViewMatrix(Entity entity, Camera& camera);

		COCOA void clearColorRgb(const Camera& camera, int attachment, glm::vec3 color);
		COCOA void clearColorUint32(Camera& camera, int attachment, uint32 color);

		COCOA glm::vec2 screenToOrtho(const Camera& camera);

		COCOA void serialize(json* j, Entity entity, const Camera& camera);
		COCOA void deserialize(const json& j, Entity entity);
	};

	namespace CameraSystem
	{
		COCOA void update(SceneData& scene, float dt);
		COCOA void destroy(SceneData& scene);
		COCOA void deleteEntity(Entity entity);
	}
}

#endif
