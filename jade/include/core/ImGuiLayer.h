#pragma once

#include "core/MenuBar.h"

#include <glm/vec2.hpp>

class ImGuiLayer {
public:
    ImGuiLayer() {}

    void Setup(void* window);
    void StartFrame();
    void ImGui();
    void Render();

    const glm::vec2& GetGameviewPos() const { return m_GameviewPos; }
    const glm::vec2& GetGameviewSize() const { return m_GameviewSize; }

private:
    void SetupDockspace();
    void RenderGameViewport();

private:
    glm::vec2 m_GameviewPos = glm::vec2();
    glm::vec2 m_GameviewSize = glm::vec2();

    void* m_Window;
    MenuBar m_MenuBar = MenuBar();
};