#pragma once
#include "externalLibs.h"
#include "cocoa/components/components.h"
#include "cocoa/components/Transform.h"
#include "cocoa/core/Handle.h"
#include "cocoa/renderer/fonts/Font.h"
#include "cocoa/renderer/Texture.h"

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

    class COCOA RenderBatch
    {
    public:
        RenderBatch(int maxBatchSize, int zIndex, bool batchOnTop=false);

        void Clear();
        void Start();
        void Add(const Transform& transform, const FontRenderer& fontRenderer);
        void Add(const Transform& transform, const SpriteRenderer& spr);
        void Add(const glm::vec2& min, const glm::vec2& max, const glm::vec3& color);
        void Add(const glm::vec2* vertices, const glm::vec3& color);
        void Add(Handle<Texture> textureHandle, const glm::vec2& size, const glm::vec2& position, 
            const glm::vec3& color, const glm::vec2& texCoordMin, const glm::vec2& texCoordMax, float rotation);
        void Render();

        inline bool BatchOnTop() { return m_BatchOnTop; }
        inline bool HasRoom() { return m_NumSprites < m_MaxBatchSize; }
        inline bool HasRoom(const FontRenderer& fontRenderer) const { return m_NumSprites + fontRenderer.text.size() < m_MaxBatchSize; }
        inline int NumSprites() { return m_NumSprites; }

        inline bool HasTextureRoom() { return m_NumTextures < m_Textures.size(); }
        inline int ZIndex() { return m_ZIndex; }

        static bool Compare(const std::shared_ptr<RenderBatch>& b1, const std::shared_ptr<RenderBatch>& b2);

        bool const HasTexture(Handle<Texture> resourceId)
        {
            for (int i = 0; i < m_NumTextures; i++)
            {
                if (m_Textures[i] == resourceId)
                {
                    return true;
                }
            }
            return false;
        }

    private:
        void LoadVertexProperties(const Transform& transform, const SpriteRenderer& spr);
        void LoadVertexProperties(const glm::vec3& position, 
            const glm::vec3& scale, const glm::vec2& quadSize, const glm::vec2* texCoords, 
            float rotationDegrees, const glm::vec4& color, int texId, uint32 entityId = -1);
        void LoadVertexProperties(const glm::vec2* vertices, const glm::vec2* texCoords, const glm::vec4& color, int texId, 
            uint32 entityId = -1);

        void LoadEmptyVertexProperties();

        void LoadElementIndices(int index);
        void GenerateIndices();

    private:
        Vertex* m_VertexBufferBase;
        Vertex* m_VertexStackPointer;
        uint32* m_Indices;
        std::array<Handle<Texture>, 16> m_Textures;

        uint32 m_VAO, m_VBO, m_EBO;
        int16 m_ZIndex = 0;
        uint16 m_NumSprites = 0;
        uint16 m_NumTextures = 0;

        int m_MaxBatchSize;
        bool m_BatchOnTop;
    };
}