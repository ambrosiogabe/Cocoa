#pragma once
#include "externalLibs.h"

#include "jade/renderer/Line2D.h"
#include "jade/renderer/Shader.h"
#include "jade/renderer/RenderBatch.h"
#include "jade/renderer/DebugSprite.h"

namespace Jade {
    class DebugDraw {
    public:
        static void BeginFrame();
        static void DrawBottomBatches();
        static void DrawTopBatches();

        static void AddLine2D(glm::vec2& from, glm::vec2& to, float strokeWidth=1.0f, glm::vec3 color={0.0f, 1.0f, 0.0f}, int lifetime=1, bool onTop=true);
        static void AddBox2D(glm::vec2& center, glm::vec2& dimensions, float rotation=0.0f, float strokeWidth=1.0f, glm::vec3 color={0.0f, 1.0f, 0.0f}, int lifetime=1, bool onTop=true);
        static void AddSprite(Texture* texture, glm::vec2 size, glm::vec2 position,
            glm::vec3 tint={1.0f, 1.0f, 1.0f}, glm::vec2 texCoordMin={0.0f, 1.0f}, glm::vec2 texCoordMax={1.0f, 0.0f}, float rotation=0.0f, int lifetime=1, bool onTop=true);

    private:
        static void RemoveDeadSprites();
        static void RemoveDeadLines();
        static void AddSpritesToBatches();
        static void AddLinesToBatches();

    private:        
        static std::vector<RenderBatch*> m_Batches;
        static std::vector<Line2D> m_Lines;
        static std::vector<DebugSprite> m_Sprites;
        static int m_TexSlots[16];
        static Shader* m_Shader;
        static int m_MaxBatchSize;
    };
}