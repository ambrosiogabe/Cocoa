#pragma once
#include "externalLibs.h"
#include "cocoa/components/Transform.h"
#include "cocoa/components/SpriteRenderer.h"
#include "cocoa/components/FontRenderer.h"
#include "cocoa/core/Handle.h"
#include "cocoa/renderer/fonts/Font.h"
#include "cocoa/renderer/Texture.h"
#include "cocoa/renderer/Shader.h"

namespace Cocoa
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec4 color;
        glm::vec2 texCoords;
        float texId;
        uint32 entityId;
    };

    struct RenderBatchData
    {
        Handle<Shader> BatchShader;
        Vertex* VertexBufferBase;
        Vertex* VertexStackPointer;
        uint32* Indices;
        std::array<Handle<Texture>, 16> Textures;

        uint32 VAO, VBO, EBO;
        int16 ZIndex = 0;
        uint16 NumSprites = 0;
        uint16 NumTextures = 0;

        int MaxBatchSize;
        bool BatchOnTop;
    };

    namespace RenderBatch
    {
        COCOA RenderBatchData CreateRenderBatch(int maxBatchSize, int zIndex, Handle<Shader> shader, bool batchOnTop=false);
        COCOA void Free(RenderBatchData& data);

        COCOA void Clear(RenderBatchData& data);
        COCOA void Start(RenderBatchData& data);
        COCOA void Add(RenderBatchData& data, const TransformData& transform, const SpriteRenderer& spr);
        COCOA void Add(RenderBatchData& data, const TransformData& transform, const FontRenderer& fontRenderer);
        COCOA void Add(RenderBatchData& data, const glm::vec2& min, const glm::vec2& max, const glm::vec3& color);
        COCOA void Add(RenderBatchData& data, const glm::vec2* vertices, const glm::vec3& color);
        COCOA void Add(RenderBatchData& data, Handle<Texture> textureHandle, const glm::vec2& size, const glm::vec2& position,
            const glm::vec3& color, const glm::vec2& texCoordMin, const glm::vec2& texCoordMax, float rotation);
        COCOA void Render(RenderBatchData& data);

        COCOA bool HasRoom(const RenderBatchData& data);
        COCOA bool HasRoom(const RenderBatchData& data, const FontRenderer& fontRenderer);
        COCOA bool HasTextureRoom(const RenderBatchData& data);

        COCOA bool Compare(const RenderBatchData& b1, const RenderBatchData& b2);

        COCOA bool HasTexture(const RenderBatchData& data, Handle<Texture> resourceId);
    };
}