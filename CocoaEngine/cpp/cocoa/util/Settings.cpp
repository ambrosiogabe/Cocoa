#include "cocoa/util/Settings.h"

namespace Cocoa
{
    namespace Settings
    {
        // =======================================================================
        // General Settings
        // =======================================================================
        int General::s_GridSizeX = 32;
        int General::s_GridSizeY = 32;
        bool General::s_DrawGrid = false;

        CPath General::s_ImGuiConfigPath = "default.ini";
        CPath General::s_EngineAssetsPath = "assets";
        CPath General::s_EngineExeDirectory = "";
        CPath General::s_EngineSourceDirectory = "";
        CPath General::s_CurrentScene = "New Scene.cocoa";
        CPath General::s_CurrentProject = "New Project.cprj";
        CPath General::s_StylesDirectory = "assets\\styles";
        CPath General::s_WorkingDirectory = "";
        CPath General::s_EditorSaveData = "EditorSaveData.json";
        CPath General::s_EditorStyleData = "EditorStyle.json";

        // =======================================================================
        // Physics Settings
        // =======================================================================
        int Physics2D::s_PositionIterations = 3;
        int Physics2D::s_VelocityIterations = 8;
        float Physics2D::s_Timestep = 1.0f / 60.0f;
    }
}