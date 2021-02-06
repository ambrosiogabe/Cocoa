#pragma once
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "editorWindows/MenuBar.h"
#include "editorWindows/AssetWindow.h"
#include "editorWindows/InspectorWindow.h"

#include "cocoa/file/CPath.h"

namespace Cocoa
{
    namespace ImGuiLayer
    {
        void Init(void* window);
        void OnEvent(Scene* scene, Event& e);
        void BeginFrame(Scene* scene);
        void EndFrame();

        void LoadStyle(const CPath& filepath);
    };
}