#pragma once
#include "externalLibs.h"

#include "jade/core/Core.h"
#include "jade/util/Log.h"

namespace Jade
{
    class DebugSprite
    {
    public:
        DebugSprite(Texture* texture, const glm::vec2& size, const glm::vec2& position, const glm::vec3& tint, 
            const glm::vec2& texCoordMin, const glm::vec2& texCoordMax, float rotation, int lifetime)
        {

            m_Texture = texture;
            m_Size = size;
            m_Position = position;
            m_Tint = tint;
            m_TexCoordMin = texCoordMin;
            m_TexCoordMax = texCoordMax;
            m_Rotation = rotation;
            m_Lifetime = lifetime;
        }

        int BeginFrame() { return m_Lifetime--; }


    public:
        Texture* m_Texture;
        glm::vec2 m_Size;
        glm::vec2 m_Position;
        glm::vec3 m_Tint;
        glm::vec2 m_TexCoordMin;
        glm::vec2 m_TexCoordMax;
        float m_Rotation;
        uint32 m_Lifetime;
    };
}