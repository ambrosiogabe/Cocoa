#pragma once
#include "externalLibs.h"
#include "cocoa/components/TransformStruct.h"
#include "cocoa/renderer/Framebuffer.h"

namespace Cocoa
{
	struct Camera
	{
		// Projection/View matrices for ortho and perspective
		glm::mat4 ViewMatrix;
		glm::mat4 InverseView;
		glm::mat4 ProjectionMatrix;
		glm::mat4 InverseProjection;

		glm::vec3 ClearColor;
		glm::vec2 ProjectionSize;
		float ProjectionNearPlane;
		float ProjectionFarPlane;
		Framebuffer Framebuffer;

		float Fov = 45.0f;
		float Aspect = 1920.0f / 1080.0f;
		float Zoom = 1.0f;
	};
}