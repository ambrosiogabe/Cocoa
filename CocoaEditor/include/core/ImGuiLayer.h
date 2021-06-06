#ifndef COCOA_EDITOR_IMGUI_LAYER_H
#define COCOA_EDITOR_IMGUI_LAYER_H
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "cocoa/file/Path.h"
#include "cocoa/scenes/SceneData.h"
#include "cocoa/events/Event.h"

namespace Cocoa
{
    namespace ImGuiLayer
    {
        void init(void* window);
        void destroy();
        void onEvent(SceneData& scene, Event& e);
        void beginFrame(SceneData& scene);
        void endFrame();

        void loadStyle(const Path& filepath);
        void applyStyle();
        void exportCurrentStyle(const Path& outputPath);
    };
}

#endif
