#ifndef COCOA_ENGINE_DEBUG_SPRITE_H
#define COCOA_ENGINE_DEBUG_SPRITE_H
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "cocoa/core/Handle.h"
#include "cocoa/renderer/Texture.h"

namespace Cocoa
{
    struct DebugSprite
    {
        Handle<Texture> spriteTexture;
        glm::vec2 size;
        glm::vec2 position;
        glm::vec3 tint;
        glm::vec2 texCoordMin;
        glm::vec2 texCoordMax;
        float rotation;
        int lifetime;
        bool onTop;
    };
}

#endif
