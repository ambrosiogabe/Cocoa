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
        std::string General::s_CurrentScene = "New Scene.jade";
        std::string General::s_CurrentProject = "New Project.prj";
        std::string General::s_WorkingDirectory = "";
        std::string General::s_EditorSaveData = "EditorSaveData.json";
        std::string General::s_EditorStyleData = "EditorStyle.json";

        // =======================================================================
        // Physics Settings
        // =======================================================================
        int Physics2D::s_PositionIterations = 3;
        int Physics2D::s_VelocityIterations = 8;
        float Physics2D::s_Timestep = 1.0f / 60.0f;

        // =======================================================================
        // Editor Style Settings
        // =======================================================================
        glm::vec4 EditorStyle::s_DarkBgColor { 0.18f, 0.18f, 0.18f, 1.00f };
        glm::vec4 EditorStyle::s_LightBgColor { 0.30f, 0.30f, 0.30f, 1.00f };
        glm::vec4 EditorStyle::s_DarkInset { 0.13f, 0.13f, 0.13f, 1.00f };

        glm::vec4 EditorStyle::s_DarkAccentColor { 0.15f, 0.55f, 0.38f, 1.00f };
        glm::vec4 EditorStyle::s_LightAccentColor { 0.45f, 0.85f, 0.69f, 1.00f };
    }
}