#pragma once
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "cocoa/util/Settings.h"
#include "cocoa/scenes/Scene.h"
#include "cocoa/file/CPath.h"

namespace Cocoa
{
    class MenuBar
    {
    public:
        MenuBar(Scene* scene) 
            : m_Scene(scene) {}

        void ImGui();
        
    public:
        static bool ShowDemoWindow;

    private:
        void SettingsWindow();
        void StylesWindow();

        static bool CPathVectorGetter(void* data, int n, const char** out_text);

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