#pragma once

class ImGuiLayer {
public:
    ImGuiLayer() {}

    void Setup(void* window);
    void StartFrame();
    void ImGui();
    void Render();

private:
    void SetupDockspace();
    void RenderGameViewport();

private:
    void* m_Window;

};