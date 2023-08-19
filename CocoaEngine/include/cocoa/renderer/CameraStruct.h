#ifndef COCOA_ENGINE_CAMERA_STRUCT_H
#define COCOA_ENGINE_CAMERA_STRUCT_H
#include "externalLibs.h"
#include "cocoa/renderer/Framebuffer.h"

namespace Cocoa
{
	struct Camera
	{
		// Projection/View matrices for ortho and perspective
		glm::mat4 viewMatrix;
		glm::mat4 inverseView;
		glm::mat4 projectionMatrix;
		glm::mat4 inverseProjection;

		glm::vec3 clearColor;
		glm::vec2 projectionSize;
		float projectionNearPlane;
		float projectionFarPlane;
		Framebuffer framebuffer;

		float fov = 45.0f;
		float aspect = 1920.0f / 1080.0f;
		float zoom = 1.0f;
	};
}

#endif
