#ifndef COCOA_ENGINE_CAMERA_DEBUG_H
#define COCOA_ENGINE_CAMERA_DEBUG_H
#include "externalLibs.h"

#include "cocoa/core/Handle.h"
#include "cocoa/renderer/Texture.h"
#include "cocoa/renderer/Camera.h"

namespace Cocoa
{
	namespace DebugDraw
	{
		COCOA void init();
		COCOA void destroy();

		COCOA void beginFrame();
		COCOA void drawBottomBatches(const Camera& camera);
		COCOA void drawTopBatches(const Camera& camera);
		COCOA void addDebugObjectsToBatches();
		COCOA void clearAllBatches();

		COCOA void addLine2D(
			glm::vec2& from,
			glm::vec2& to,
			float strokeWidth = 1.2f,
			glm::vec3 color = { 0.0f, 1.0f, 0.0f },
			int lifetime = 1,
			bool onTop = true
		);

		COCOA void addBox2D(
			glm::vec2& center,
			glm::vec2& dimensions,
			float rotation = 0.0f,
			float strokeWidth = 0.01f,
			glm::vec3 color = { 0.0f, 1.0f, 0.0f },
			int lifetime = 1,
			bool onTop = true
		);

		COCOA void addFilledBox(
			const glm::vec2& center,
			const glm::vec2& dimensions,
			float rotation = 0.0f,
			const glm::vec3 color = { 0.0f, 1.0f, 0.0f },
			int lifetime = 1,
			bool onTop = true
		);

		COCOA void addSprite(
			Handle<Texture> spriteTexture,
			glm::vec2 size,
			glm::vec2 position,
			glm::vec3 tint = { 1.0f, 1.0f, 1.0f },
			glm::vec2 texCoordMin = { 0.0f, 1.0f },
			glm::vec2 texCoordMax = { 1.0f, 0.0f },
			float rotation = 0.0f,
			int lifetime = 1,
			bool onTop = true
		);

		COCOA void addShape(
			const glm::vec2* vertices,
			int numVertices,
			int numElements,
			const glm::vec3& color,
			const glm::vec2& position,
			const glm::vec2& scale = { 1.0f, 1.0f },
			float rotation = 0.0f,
			int lifetime = 1,
			bool onTop = true
		);
	};
}

#endif
