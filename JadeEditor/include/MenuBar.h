#pragma once

#include "jade/util/Settings.h"
#include "jade/scenes/Scene.h"

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

    private:
        Scene* m_Scene;
        bool m_SettingsOpen = false;
        bool m_CreatingProject = false;

        // -------------Settings Variables-------------------------
        int m_GridSize[2] = { Settings::General::s_GridSizeX, Settings::General::s_GridSizeY };
        bool m_SnapToGrid = false;
        // ---------------------------------------------------------
    };
}