#include "cocoa/util/Settings.h"

namespace Cocoa
{
    namespace Settings
    {
        // =======================================================================
        // General Settings
        // =======================================================================
        CPath General::s_ImGuiConfigPath = NCPath::CreatePath("default.ini");
        CPath General::s_EngineAssetsPath = NCPath::CreatePath("assets");
        CPath General::s_EngineExeDirectory = NCPath::CreatePath();
        CPath General::s_EngineSourceDirectory = NCPath::CreatePath();
        CPath General::s_CurrentScene = NCPath::CreatePath("New Scene.cocoa");
        CPath General::s_CurrentProject = NCPath::CreatePath("New Project.cprj");
        CPath General::s_StylesDirectory = NCPath::CreatePath("assets\\styles");
        CPath General::s_WorkingDirectory = NCPath::CreatePath();
        CPath General::s_EditorSaveData = NCPath::CreatePath("EditorSaveData.json");
        CPath General::s_EditorStyleData = NCPath::CreatePath("EditorStyle.json");

        // =======================================================================
        // Physics Settings
        // =======================================================================
        int Physics2D::s_PositionIterations = 3;
        int Physics2D::s_VelocityIterations = 8;
        float Physics2D::s_Timestep = 1.0f / 60.0f;
    }
}