#pragma once
#include "externalLibs.h"
#include "cocoa/core/Core.h"

namespace Cocoa
{
    struct DebugShape
    {
        glm::vec2* Vertices;
        int NumVertices;
        glm::vec3 Color;
        glm::vec2 Position;
        float Rotation;
        int Lifetime;
        bool OnTop;
    };
}