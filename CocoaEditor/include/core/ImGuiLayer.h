#ifndef COCOA_EDITOR_IMGUI_LAYER_H
#define COCOA_EDITOR_IMGUI_LAYER_H
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "cocoa/scenes/SceneData.h"
#include "cocoa/events/Event.h"

#include <filesystem>

namespace Cocoa
{
    namespace ImGuiLayer
    {
        void init(void* window);
        void destroy();
        void onEvent(SceneData& scene, Event& e);
        void beginFrame(SceneData& scene);
        void endFrame();

        void loadStyle(const std::filesystem::path& filepath);
        void applyStyle();
        void exportCurrentStyle(const std::filesystem::path& outputPath);
    };
}

#endif
