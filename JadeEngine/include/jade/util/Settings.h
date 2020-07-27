#pragma once
#include "externalLibs.h"

namespace Jade {
    namespace Settings
    {
        class General
        {
        public:
            static int s_GridSizeX;
            static int s_GridSizeY;
            static bool s_DrawGrid;
            static std::string s_WorkingDirectory;
            static std::string s_CurrentProject;
            static std::string s_CurrentScene;
            static std::string s_EditorSaveData;
            static std::string s_EditorStyleData;
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