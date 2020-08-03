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
        JPath General::s_CurrentScene = "New Scene.jade";
        JPath General::s_CurrentProject = "New Project.prj";
        JPath General::s_WorkingDirectory = "";
        JPath General::s_EditorSaveData = "EditorSaveData.json";
        JPath General::s_EditorStyleData = "EditorStyle.json";

        // =======================================================================
        // Physics Settings
        // =======================================================================
        int Physics2D::s_PositionIterations = 3;
        int Physics2D::s_VelocityIterations = 8;
        float Physics2D::s_Timestep = 1.0f / 60.0f;

        // =======================================================================
        // Editor Style Settings
        // =======================================================================
        // Fonts
        ImFont* EditorStyle::s_DefaultFont = nullptr;
        ImFont* EditorStyle::s_LargeIconFont = nullptr;

        // Style Colors
        glm::vec4 EditorStyle::s_MainBgLight0{ 0.404f, 0.404f ,0.404f, 1.0f };
        glm::vec4 EditorStyle::s_MainBg { 0.212f, 0.212f, 0.212f, 1.0f };
        glm::vec4 EditorStyle::s_MainBgDark0 { 0.190f, 0.190f, 0.190f, 1.0f };
        glm::vec4 EditorStyle::s_MainBgDark1 { 0.145f, 0.145f, 0.145f, 1.0f };
        glm::vec4 EditorStyle::s_MainBgDark2 { 0.098f, 0.098f, 0.098f, 1.0f };

        glm::vec4 EditorStyle::s_Accent{ 0.149f, 0.149f, 0.149f, 1.0f };
        glm::vec4 EditorStyle::s_AccentDark0{ 0.102f, 0.102f, 0.102f, 1.0f };
        glm::vec4 EditorStyle::s_AccentDark1{ 0.063f, 0.063f, 0.063f, 1.0f };

        glm::vec4 EditorStyle::s_Button{ 0.882f, 0.882f, 0.882f, 1.0f };
        glm::vec4 EditorStyle::s_ButtonHovered{ 0.782f, 0.782f, 0.782f, 1.0f };

        glm::vec4 EditorStyle::s_Font { 0.902f, 0.902f, 0.902f, 1.0f };
        glm::vec4 EditorStyle::s_FontDisabled { 0.839f, 0.839f, 0.839f, 1.0f };

        // Sizing
        glm::vec2 EditorStyle::s_WindowPadding{ 10, 10 };
        glm::vec2 EditorStyle::s_FramePadding{ 20, 8 };
        glm::vec2 EditorStyle::s_ItemSpacing{ 20, 8 };
        int EditorStyle::s_ScrollbarSize = 17;
        int EditorStyle::s_ScrollbarRounding = 12;
        int EditorStyle::s_FrameRounding = 8;
        int EditorStyle::s_GrabRounding = 8;
        int EditorStyle::s_TabRounding = 8;
    }
}