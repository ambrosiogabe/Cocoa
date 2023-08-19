#ifndef COCOA_ENGINE_RENDERER_2D_H
#define COCOA_ENGINE_RENDERER_2D_H
#include "externalLibs.h"

#include "cocoa/components/Transform.h"
#include "cocoa/components/SpriteRenderer.h"
#include "cocoa/components/FontRenderer.h"
#include "cocoa/core/Handle.h"
#include "cocoa/renderer/Texture.h"
#include "cocoa/renderer/Shader.h"

namespace Cocoa
{
    namespace RenderBatch
    {
        const int TEXTURE_SIZE = 16;
    }

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
        Handle<Shader> batchShader;
        Vertex* vertexBufferBase;
        Vertex* vertexStackPointer;
        uint32* indices;
        Handle<Texture> textures[RenderBatch::TEXTURE_SIZE];

        uint32 vao, vbo, ebo;
        int16 zIndex;
        uint16 numUsedElements;
        uint16 numTextures;

        int maxBatchSize;
        bool batchOnTop;
    };

    namespace RenderBatch
    {
        COCOA RenderBatchData createRenderBatch(int maxBatchSize, int zIndex, Handle<Shader> shader, bool batchOnTop=false);
        COCOA void free(RenderBatchData& data);

        COCOA void clear(RenderBatchData& data);
        COCOA void start(RenderBatchData& data);
        COCOA void add(RenderBatchData& data, const TransformData& transform, const SpriteRenderer& spr);
        COCOA void add(RenderBatchData& data, const TransformData& transform, const FontRenderer& fontRenderer);
        COCOA void add(RenderBatchData& data, const glm::vec2* vertices, const glm::vec3& color, const glm::vec2& position={0.0f, 0.0f}, int numVertices=4, int numElements=6);
        
        COCOA void add(
            RenderBatchData& data,
            Handle<Texture> textureHandle,
            const glm::vec3& position,
            const glm::vec3& scale,
            const glm::vec3& color,
            const glm::vec2& texCoordMin, 
            const glm::vec2& texCoordMax, 
            float rotation);

        COCOA void render(RenderBatchData& data);

        COCOA bool hasRoom(const RenderBatchData& data, int numVertices=4);
        COCOA bool hasRoom(const RenderBatchData& data, const FontRenderer& fontRenderer);
        COCOA bool hasTextureRoom(const RenderBatchData& data);

        COCOA bool compare(const RenderBatchData& b1, const RenderBatchData& b2);

        COCOA bool hasTexture(const RenderBatchData& data, Handle<Texture> resourceId);
    };
}

#endif
