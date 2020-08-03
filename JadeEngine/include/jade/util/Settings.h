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

            static JPath s_ImGuiConfigPath;
            static JPath s_EngineAssetsPath;
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
            // Fonts
            static ImFont* s_DefaultFont;
            static ImFont* s_LargeIconFont;

            // Style Colors
            static glm::vec4 s_MainBgLight0;
            static glm::vec4 s_MainBg;
            static glm::vec4 s_MainBgDark0;
            static glm::vec4 s_MainBgDark1;
            static glm::vec4 s_MainBgDark2;

            static glm::vec4 s_Accent;
            static glm::vec4 s_AccentDark0;
            static glm::vec4 s_AccentDark1;

            static glm::vec4 s_Button;
            static glm::vec4 s_ButtonHovered;

            static glm::vec4 s_Font;
            static glm::vec4 s_FontDisabled;

            // Sizing
            static glm::vec2 s_WindowPadding;
            static glm::vec2 s_FramePadding;
            static glm::vec2 s_ItemSpacing;
            static float s_ScrollbarSize;
            static float s_ScrollbarRounding;
            static float s_FrameRounding;
            static float s_GrabRounding;
            static float s_TabRounding;
        };
    }
}