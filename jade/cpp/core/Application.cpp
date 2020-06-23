#include "core/Application.h"

#include <string>
#include <iostream>

namespace Jade {
    Application::Application() {
        m_LayerInsertIndex = 0;
        m_Running = true;
        m_Layers = std::vector<Layer*>();

        std::string title = std::string("Test Window");
        m_Window = JWindow::Create(1920, 1080, title);
        s_Instance = this;
    }

    Application::~Application() {

    }

    void Application::Run() {
        for (Layer* layer : m_Layers) {
            layer->OnAttach();
        }

        while (m_Running) {
            float time = Window::GetTime();
            float dt = m_LastFrameTime - time;
            m_LastFrameTime = time;

            for (Layer* layer : m_Layers) {
                layer->OnUpdate(dt);
                layer->OnRender();
            }

            // ImGuiLayer->Begin();
            // for (Layer* layer : m_Layers) {
            //     layer->OnImGuiRender();
            // }
            // ImGuiLayer->End();

            m_Window->OnUpdate();
            m_Window->Render();
        }

        for (Layer* layer : m_Layers) {
            layer->OnDetach();
        }
    }

    void Application::PushLayer(Layer* layer) {
        m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
        m_LayerInsertIndex++;
    }

    void Application::PushOverlay(Layer* overlay) {
        m_Layers.emplace_back(overlay);
    }

    void Application::PopLayer(Layer* layer) {
        auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
        if (it != m_Layers.end()) {
            m_Layers.erase(it);
            m_LayerInsertIndex--;
        }
    }

    void Application::PopOverlay(Layer* overlay) {
        auto it = std::find(m_Layers.begin(), m_Layers.end(), overlay);
        if (it != m_Layers.end()) {
            m_Layers.erase(it);
        }
    }

    Framebuffer* Application::GetFramebuffer() const {
        return m_Framebuffer;
    }

    void Application::ChangeScene(Scene* newScene) {
        this->m_CurrentScene = newScene;
        this->m_CurrentScene->Init();
        this->m_CurrentScene->Start();
    }

    Scene* Application::GetScene() const {
        return m_CurrentScene;
    }

    void Application::Stop() {
        m_Running = false;
    }

    Application* Application::s_Instance = nullptr;
    Application* Application::Get() {
        Log::Assert(s_Instance != nullptr, "Cannot get application. It is nullptr.");
        return s_Instance;
    }
}