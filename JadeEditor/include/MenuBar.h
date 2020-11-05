#pragma once

#include "jade/util/Settings.h"
#include "jade/scenes/Scene.h"
#include "jade/file/JPath.h"

namespace Jade
{
    class MenuBar
    {
    public:
        MenuBar(Scene* scene) 
            : m_Scene(scene) {}

        void ImGui();

    private:
        void SettingsWindow();
        void StylesWindow();

        static bool JPathVectorGetter(void* data, int n, const char** out_text);

    private:
        Scene* m_Scene;
        bool m_SettingsOpen = false;
        bool m_StyleSelectOpen = false;
        bool m_CreatingProject = false;

        // -------------Settings Variables-------------------------
        int m_GridSize[2] = { Settings::General::s_GridSizeX, Settings::General::s_GridSizeY };
        bool m_SnapToGrid = false;
        // --------------------------------------------------------

        // -------------Style Settings-----------------------------
        int m_SelectedStyle = 0;
        // --------------------------------------------------------
    };
}