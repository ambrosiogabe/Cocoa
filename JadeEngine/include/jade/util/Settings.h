#pragma once
#include "externalLibs.h"
#include "jade/file/JPath.h"

namespace Jade {
    namespace Settings
    {
        class JADE General
        {
        public:
            static int s_GridSizeX;
            static int s_GridSizeY;
            static bool s_DrawGrid;

            static JPath s_ImGuiConfigPath;
            static JPath s_EngineAssetsPath;
            static JPath s_WorkingDirectory;
            static JPath s_CurrentProject;
            static JPath s_StylesDirectory;
            static JPath s_CurrentScene;
            static JPath s_EditorSaveData;
            static JPath s_EditorStyleData;
            static JPath s_EditorStyle;
        };
  
        class JADE Physics2D
        {
        public:
            static int s_VelocityIterations;
            static int s_PositionIterations;
            static float s_Timestep;
        };
    }
}