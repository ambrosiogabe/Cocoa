#pragma once
#include "externalLibs.h"

#include "cocoa/core/Core.h"
#include "cocoa/core/Handle.h"
#include "cocoa/renderer/Texture.h"

namespace Cocoa
{
    struct DebugSprite
    {
        Handle<Texture> SpriteTexture;
        glm::vec2 Size;
        glm::vec2 Position;
        glm::vec3 Tint;
        glm::vec2 TexCoordMin;
        glm::vec2 TexCoordMax;
        float Rotation;
        int Lifetime;
        bool OnTop;
    };

    //namespace NDebugSprite
    //{
    //    COCOA DebugSprite Create(
    //        Handle<Texture> textureAssetId,
    //        const glm::vec2& size,
    //        const glm::vec2& position,
    //        const glm::vec3& tint,
    //        const glm::vec2& texCoordMin,
    //        const glm::vec2& texCoordMax,
    //        float rotation,
    //        int lifetime,
    //        bool onTop);
    //}
}