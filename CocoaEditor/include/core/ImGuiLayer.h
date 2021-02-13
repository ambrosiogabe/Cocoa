#pragma once
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "editorWindows/MenuBar.h"
#include "editorWindows/AssetWindow.h"
#include "editorWindows/InspectorWindow.h"

#include "cocoa/file/CPath.h"
#include "cocoa/scenes/SceneData.h"
#include "cocoa/events/Event.h"

namespace Cocoa
{
    namespace ImGuiLayer
    {
        void Init(void* window);
        void OnEvent(SceneData& scene, Event& e);
        void BeginFrame(SceneData& scene);
        void EndFrame();

        void LoadStyle(const CPath& filepath);
    };
}