#pragma once

#include "util/Constants.h"

class MenuBar {
public:
    void ImGui();

private:
    void SettingsWindow();

private:
    bool m_SettingsOpen = false;

    // -------------Settings Variables-------------------------
    int m_GridSize[2] = {Constants::GridSizeX, Constants::GridSizeY};
    bool m_SnapToGrid = false;
    // ---------------------------------------------------------
};