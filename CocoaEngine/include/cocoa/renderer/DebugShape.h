#ifndef COCOA_ENGINE_DEBUG_SHAPE_H
#define COCOA_ENGINE_DEBUG_SHAPE_H
#include "externalLibs.h"
#include "cocoa/core/Core.h"

namespace Cocoa
{
    struct DebugShape
    {
        glm::vec2* vertices;
        int numVertices;
        int numElements;
        glm::vec3 color;
        glm::vec2 position;
        float rotation;
        int lifetime;
        bool onTop;
    };
}

#endif
