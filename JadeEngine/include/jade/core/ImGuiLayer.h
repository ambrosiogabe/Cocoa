#pragma once

#include "jade/core/MenuBar.h"
#include "jade/core/Layer.h"

#include <glm/vec2.hpp>

namespace Jade {
    class ImGuiLayer : public Layer {
    public:
        ImGuiLayer() {
            m_Window = nullptr;
        }

        void Setup(void* window);

        virtual void OnEvent(Event& e) override;
        void BeginFrame();
        void EndFrame();

        const glm::vec2& GetGameViewPos() const { return m_GameviewPos; }
        const glm::vec2& GetGameViewSize() const { return m_GameviewSize; }
        const glm::vec2& GetGameViewMousePos() const { return m_GameviewMousePos; }

    private:
        void SetupDockspace();
        void RenderGameViewport();

    private:
        glm::vec2 m_GameviewPos = glm::vec2();
        glm::vec2 m_GameviewSize = glm::vec2();
        glm::vec2 m_GameviewMousePos = glm::vec2();
        bool m_BlockEvents = false;

        void* m_Window;
        MenuBar m_MenuBar = MenuBar();
    };
}