#pragma once
#include "externalLibs.h"

#include "cocoa/core/Core.h"
#include "cocoa/util/Log.h"

namespace Cocoa
{
    class COCOA DebugSprite
    {
    public:
        DebugSprite(uint32 textureAssetId, const glm::vec2& size, const glm::vec2& position, const glm::vec3& tint, 
            const glm::vec2& texCoordMin, const glm::vec2& texCoordMax, float rotation, int lifetime, bool onTop)
        {

            m_TextureAssetId = textureAssetId;
            m_Size = size;
            m_Position = position;
            m_Tint = tint;
            m_TexCoordMin = texCoordMin;
            m_TexCoordMax = texCoordMax;
            m_Rotation = rotation;
            m_Lifetime = lifetime;
            m_OnTop = onTop;
        }

        int BeginFrame() { return m_Lifetime--; }


    public:
        uint32 m_TextureAssetId;
        glm::vec2 m_Size;
        glm::vec2 m_Position;
        glm::vec3 m_Tint;
        glm::vec2 m_TexCoordMin;
        glm::vec2 m_TexCoordMax;
        float m_Rotation;
        uint32 m_Lifetime;
        bool m_OnTop;
    };
}