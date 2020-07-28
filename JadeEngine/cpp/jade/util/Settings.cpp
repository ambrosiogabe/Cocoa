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
        glm::vec4 EditorStyle::s_DarkBgColor { 0.122f, 0.122f, 0.122f, 1.00f };
        glm::vec4 EditorStyle::s_LightBgColor { 0.259f, 0.259f, 0.259f, 1.00f };
        glm::vec4 EditorStyle::s_LightColorHover{ 0.33f, 0.33f, 0.33f, 1.00f };
        glm::vec4 EditorStyle::s_DarkInset{ 56.0f / 255.0f, 56.0f / 255.0f, 56.0f / 255.0f, 1.0f };
        glm::vec4 EditorStyle::s_TitleBgColor{ 0.137f, 0.137f, 0.137f, 1.0f };
        glm::vec4 EditorStyle::s_MenubarColor{ 0.137f, 0.137f, 0.137f, 1.0f };

        glm::vec4 EditorStyle::s_InputElementColor{ 0.349f, 0.349f, 0.349f, 1.0f };
        glm::vec4 EditorStyle::s_TabColor{ 0.169f, 0.169f, 0.169f, 1.0f };
        glm::vec4 EditorStyle::s_TabActiveColor{0.259f, 0.259f, 0.259f, 1.0f};
        glm::vec4 EditorStyle::s_TreeItemDark{0.157f, 0.157f, 0.175f, 1.0f};
        glm::vec4 EditorStyle::s_TreeItemLight{ 0.176f, 0.176f, 0.176f, 1.0f };

        glm::vec4 EditorStyle::s_DarkAccentColor { 0.15f, 0.55f, 0.38f, 1.00f };
        glm::vec4 EditorStyle::s_LightAccentColor { 0.45f, 0.85f, 0.69f, 1.00f };

        glm::vec2 EditorStyle::s_WindowPadding{ 0, 0 };
        glm::vec2 EditorStyle::s_FramePadding{ 20, 6 };
        glm::vec2 EditorStyle::s_ItemSpacing{ 20, 8 };
        int EditorStyle::s_ScrollbarSize = 17;
        int EditorStyle::s_ScrollbarRounding = 12;
        int EditorStyle::s_FrameRounding = 8;
        int EditorStyle::s_GrabRounding = 8;
        int EditorStyle::s_TabRounding = 8;
    }
}