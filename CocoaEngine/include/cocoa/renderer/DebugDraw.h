#pragma once
#include "externalLibs.h"

#include "cocoa/scenes/SceneData.h"
#include "cocoa/core/Handle.h"
#include "cocoa/renderer/Texture.h"
#include "cocoa/renderer/Camera.h"

namespace Cocoa
{
	namespace DebugDraw
	{
		COCOA void Init();
		COCOA void Destroy();

		COCOA void BeginFrame();
		COCOA void DrawBottomBatches(const Camera& camera);
		COCOA void DrawTopBatches(const Camera& camera);

		COCOA void AddLine2D(
			glm::vec2& from, 
			glm::vec2& to, 
			float strokeWidth = 1.2f, 
			glm::vec3 color = { 0.0f, 1.0f, 0.0f }, 
			int lifetime = 1, 
			bool onTop = true
		);

		COCOA void AddBox2D(
			glm::vec2& center, 
			glm::vec2& dimensions, 
			float rotation = 0.0f, 
			float strokeWidth = 1.2f, 
			glm::vec3 color = { 0.0f, 1.0f, 0.0f }, 
			int lifetime = 1, 
			bool onTop = true
		);

		COCOA void AddSprite(
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

		COCOA void AddShape(
			glm::vec2* vertices,
			int numVertices,
			glm::vec3& color,
			const glm::vec2& position,
			float rotation = 0.0f,
			int lifetime = 1,
			bool onTop = true
		);
	};
}