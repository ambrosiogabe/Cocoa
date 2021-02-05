#pragma once
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "cocoa/util/Settings.h"
#include "cocoa/scenes/Scene.h"
#include "cocoa/file/CPath.h"

namespace Cocoa
{
    struct MenuBar
    {
        Scene* ScenePtr;
    };

    namespace MenuBarUtil
    {        
        void ImGui(const MenuBar& menuBar);
    };
}