#pragma once
#include "externalLibs.h"
#include "jade/components/components.h"
#include "jade/components/Transform.h"

namespace Jade
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec4 color;
        glm::vec2 texCoords;
        float texId;
    };

    class RenderBatch
    {
    public:
        RenderBatch(int maxBatchSize);

        void Clear();
        void Start();
        void Add(const Transform& transform, const SpriteRenderer& spr);
        void Add(const glm::vec2& min, const glm::vec2& max, const glm::vec3& color);
        void Add(uint32 texId, const glm::vec2& size, const glm::vec2& position, 
            const glm::vec3& color, const glm::vec2& texCoordMin, const glm::vec2& texCoordMax, float rotation);
        void Render();

        bool const HasRoom()
        {
            return m_NumSprites < m_MaxBatchSize;
        }

        int const NumSprites()
        {
            return m_NumSprites;
        }

        bool const HasTextureRoom()
        {
            return m_NumTextures < 16;
        }

        bool const HasTexture(Texture* tex)
        {
            for (int i = 0; i < 16; i++)
            {
                if (m_Textures[i] == tex)
                {
                    return true;
                }
            }
            return false;
        }

        bool const HasTextureId(uint32 id)
        {
            for (int i = 0; i < m_NumTextures; i++)
            {
                if (m_Textures[i]->GetId() == id)
                {
                    return true;
                }
            }
            return false;
        }

    private:
        void LoadVertexProperties(const Transform& transform, const SpriteRenderer& spr);
        void LoadVertexProperties(const glm::vec3& position, 
            const glm::vec3& scale, const glm::vec2& quadSize, glm::vec2* texCoords, 
            float rotationDegrees, const glm::vec4& color, int texId);

        void LoadEmptyVertexProperties();

        void LoadElementIndices(int index);
        void GenerateIndices();

    private:
        Vertex* m_VertexBufferBase;
        Vertex* m_VertexStackPointer;
        uint32* m_Indices;
        std::array<Texture*, 16> m_Textures;

        uint32 m_VAO, m_VBO, m_EBO;
        uint16 m_ZIndex = 0;
        uint16 m_NumSprites = 0;
        uint16 m_NumTextures = 0;

        int m_MaxBatchSize;
    };
}