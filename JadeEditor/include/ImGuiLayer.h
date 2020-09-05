#pragma once
#include "externalLibs.h"

#include "MenuBar.h"
#include "EditorWindows/AssetWindow.h"
#include "jade/core/Layer.h"

namespace Jade
{
    class ImGuiLayer : public Layer
    {
    public:
        ImGuiLayer(Scene* scene)
            : Layer(scene), m_AssetWindow(scene)
        {
            m_Window = nullptr;
            m_MenuBar = std::make_unique<MenuBar>(scene);
        }

        void Setup(void* window);

        virtual void OnAttach() override;
        virtual void OnEvent(Event& e) override;
        void BeginFrame();
        void EndFrame();

    private:
        void SetupDockspace();
        void RenderGameViewport();

    public:
        static void LoadStyle(const JPath& filepath);

    private:
        glm::vec2 m_GameviewPos = glm::vec2();
        glm::vec2 m_GameviewSize = glm::vec2();
        glm::vec2 m_GameviewMousePos = glm::vec2();
        bool m_BlockEvents = false;

        void* m_Window;
        std::unique_ptr<MenuBar> m_MenuBar;
        AssetWindow m_AssetWindow;
    };
}