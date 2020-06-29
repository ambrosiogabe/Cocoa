#pragma once

#include "jade/core/MenuBar.h"
#include "jade/core/Layer.h"

#include <glm/vec2.hpp>
#include <GLFW/glfw3.h>

namespace Jade {
    class ImGuiLayer : public Layer {
    public:
        ImGuiLayer() {}

        void Setup(GLFWwindow* window);
        void BeginFrame();
        void EndFrame();

        const glm::vec2& GetGameViewPos() const { return m_GameviewPos; }
        const glm::vec2& GetGameViewSize() const { return m_GameviewSize; }

    private:
        void SetupDockspace();
        void RenderGameViewport();

    private:
        glm::vec2 m_GameviewPos = glm::vec2();
        glm::vec2 m_GameviewSize = glm::vec2();

        GLFWwindow* m_Window;
        MenuBar m_MenuBar = MenuBar();
    };
}