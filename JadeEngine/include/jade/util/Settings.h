#pragma once
#include "externalLibs.h"
#include "jade/file/JPath.h"

namespace Jade {
    namespace Settings
    {
        class General
        {
        public:
            static int s_GridSizeX;
            static int s_GridSizeY;
            static bool s_DrawGrid;
            static JPath s_WorkingDirectory;
            static JPath s_CurrentProject;
            static JPath s_CurrentScene;
            static JPath s_EditorSaveData;
            static JPath s_EditorStyleData;
        };
  
        class Physics2D
        {
        public:
            static int s_VelocityIterations;
            static int s_PositionIterations;
            static float s_Timestep;
        };

        class EditorStyle
        {
        public:
            static glm::vec4 s_DarkBgColor;
            static glm::vec4 s_LightBgColor;
            static glm::vec4 s_DarkInset;
            static glm::vec4 s_TitleBgColor;
            static glm::vec4 s_MenubarColor;
            static glm::vec4 s_LightColorHover;
            static glm::vec4 s_TabColor;
            static glm::vec4 s_TabActiveColor;

            static glm::vec4 s_InputElementColor;
            static glm::vec4 s_DarkAccentColor;
            static glm::vec4 s_LightAccentColor;
            static glm::vec4 s_TreeItemDark;
            static glm::vec4 s_TreeItemLight;

            // Fonts
            static ImFont* s_DefaultFont;
            static ImFont* s_LargeIconFont;

            // New Style Colors
            static glm::vec4 s_MainBgLight0;
            static glm::vec4 s_MainBg;
            static glm::vec4 s_MainBgDark0;
            static glm::vec4 s_MainBgDark1;
            static glm::vec4 s_MainBgDark2;

            static glm::vec4 s_Accent;
            static glm::vec4 s_DarkAccent0;
            static glm::vec4 s_DarkAccent1;

            static glm::vec4 s_Button;
            static glm::vec4 s_ButtonHovered;

            static glm::vec4 s_Font;
            static glm::vec4 s_FontDisabled;

            // Sizing
            static glm::vec2 s_WindowPadding;
            static glm::vec2 s_FramePadding;
            static glm::vec2 s_ItemSpacing;
            static int s_ScrollbarSize;
            static int s_ScrollbarRounding;
            static int s_FrameRounding;
            static int s_GrabRounding;
            static int s_TabRounding;
        };
    }
}