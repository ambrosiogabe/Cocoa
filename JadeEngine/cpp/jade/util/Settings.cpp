#include "jade/util/Settings.h"

namespace Jade
{
    namespace Settings
    {
        // =======================================================================
        // General Settings
        // =======================================================================
        int General::s_GridSizeX = 32;
        int General::s_GridSizeY = 32;
        bool General::s_DrawGrid = false;

        JPath General::s_ImGuiConfigPath = "default.ini";
        JPath General::s_EngineAssetsPath = "assets";
        JPath General::s_CurrentScene = "New Scene.jade";
        JPath General::s_CurrentProject = "New Project.prj";
        JPath General::s_StylesDirectory = "assets\\styles";
        JPath General::s_WorkingDirectory = "";
        JPath General::s_EditorSaveData = "EditorSaveData.json";
        JPath General::s_EditorStyleData = "EditorStyle.json";

        // =======================================================================
        // Physics Settings
        // =======================================================================
        int Physics2D::s_PositionIterations = 3;
        int Physics2D::s_VelocityIterations = 8;
        float Physics2D::s_Timestep = 1.0f / 60.0f;
    }
}