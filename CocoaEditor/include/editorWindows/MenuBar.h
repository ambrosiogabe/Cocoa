#pragma once
#include "cocoa/core/Core.h"
#include "externalLibs.h"

namespace Cocoa
{
    class Scene;
    struct MenuBar
    {
        Scene* ScenePtr;
    };

    namespace MenuBarUtil
    {        
        void ImGui(const MenuBar& menuBar);
    };
}