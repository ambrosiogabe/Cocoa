#pragma once
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "gui/ImGuiHeader.h"

#include "cocoa/file/CPath.h"

namespace Cocoa
{
	namespace Settings
	{
        namespace Editor
        {
            // Which windows are open (of the windows that can be closed)
            extern bool ShowDemoWindow;
            extern bool ShowSettingsWindow;
            extern bool ShowStyleSelect;

            // Grid stuff
            extern bool SnapToGrid;
            extern bool DrawGrid;
            extern glm::ivec2 GridSize;

            // Selected style
            extern int SelectedStyle;
        };

        namespace EditorStyle
        {
            // Fonts
            extern ImFont* s_DefaultFont;
            extern ImFont* s_LargeIconFont;

            // Style Colors
            extern glm::vec4 s_MainBgLight0;
            extern glm::vec4 s_MainBg;
            extern glm::vec4 s_MainBgDark0;
            extern glm::vec4 s_MainBgDark1;
            extern glm::vec4 s_MainBgDark2;

            extern glm::vec4 s_Accent;
            extern glm::vec4 s_AccentDark0;
            extern glm::vec4 s_AccentDark1;

            extern glm::vec4 s_Button;
            extern glm::vec4 s_ButtonHovered;

            extern glm::vec4 s_Font;
            extern glm::vec4 s_FontDisabled;
            extern glm::vec4 s_HighlightColor;

            // Sizing
            extern glm::vec2 s_WindowPadding;
            extern glm::vec2 s_FramePadding;
            extern glm::vec2 s_ItemSpacing;
            extern float s_ScrollbarSize;
            extern float s_ScrollbarRounding;
            extern float s_FrameRounding;
            extern float s_GrabRounding;
            extern float s_TabRounding;
        };
	}
}