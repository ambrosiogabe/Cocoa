#pragma once
#include "externalLibs.h"

#include "cocoa/renderer/Line2D.h"
#include "cocoa/renderer/Shader.h"
#include "cocoa/renderer/RenderBatch.h"
#include "cocoa/renderer/DebugSprite.h"
#include "cocoa/core/Handle.h"

namespace Cocoa
{
	class COCOA DebugDraw
	{
	public:
		static void BeginFrame();
		static void DrawBottomBatches();
		static void DrawTopBatches();

		static void AddLine2D(glm::vec2& from, glm::vec2& to, float strokeWidth = 1.2f, glm::vec3 color = { 0.0f, 1.0f, 0.0f }, int lifetime = 1, bool onTop = true);
		static void AddBox2D(glm::vec2& center, glm::vec2& dimensions, float rotation = 0.0f, float strokeWidth = 1.2f, glm::vec3 color = { 0.0f, 1.0f, 0.0f }, int lifetime = 1, bool onTop = true);
		static void AddSprite(uint32 textureAssetId, glm::vec2 size, glm::vec2 position,
			glm::vec3 tint = { 1.0f, 1.0f, 1.0f }, glm::vec2 texCoordMin = { 0.0f, 1.0f }, glm::vec2 texCoordMax = { 1.0f, 0.0f }, float rotation = 0.0f,
			int lifetime = 1, bool onTop = true);

		static void Init(Scene* scene);

	private:
		static void RemoveDeadSprites();
		static void RemoveDeadLines();
		static void AddSpritesToBatches();
		static void AddLinesToBatches();

	private:
		static std::vector<std::shared_ptr<RenderBatch>> s_Batches;
		static std::vector<Line2D> s_Lines;
		static std::vector<DebugSprite> s_Sprites;
		static int s_TexSlots[16];
		static Handle<Shader> s_Shader;
		static int s_MaxBatchSize;
		static Scene* s_Scene;
	};
}