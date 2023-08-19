#ifndef COCOA_EDITOR_SETTINGS_H
#define COCOA_EDITOR_SETTINGS_H
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "gui/ImGuiHeader.h"

namespace Cocoa
{
    namespace Settings
    {
        namespace Editor
        {
            // Which windows are open (of the windows that can be closed)
            extern bool showDemoWindow;
            extern bool showSettingsWindow;
            extern bool showStyleSelect;

            // Grid stuff
            extern bool snapToGrid;
            extern bool drawGrid;
            extern glm::vec2 gridSize;
            extern glm::vec3 gridColor;
            extern float gridStrokeWidth;

            // Selected style
            extern int selectedStyle;
        };

        namespace EditorStyle
        {
            // Fonts
            extern ImFont* defaultFont;
            extern ImFont* largeIconFont;

            // Style Colors
            extern glm::vec4 mainBgLight0;
            extern glm::vec4 mainBg;
            extern glm::vec4 mainBgDark0;
            extern glm::vec4 mainBgDark1;
            extern glm::vec4 mainBgDark2;

            extern glm::vec4 accent;
            extern glm::vec4 accentDark0;
            extern glm::vec4 accentDark1;

            extern glm::vec4 button;
            extern glm::vec4 buttonHovered;

            extern glm::vec4 header;
            extern glm::vec4 headerHovered;
            extern glm::vec4 headerActive;

            extern glm::vec4 font;
            extern glm::vec4 fontDisabled;
            extern glm::vec4 highlightColor;

            // Sizing
            extern glm::vec2 windowPadding;
            extern glm::vec2 framePadding;
            extern glm::vec2 itemSpacing;
            extern float scrollbarSize;
            extern float scrollbarRounding;
            extern float frameRounding;
            extern float grabRounding;
            extern float tabRounding;
        };
    }
}

#endif
