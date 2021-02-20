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
}