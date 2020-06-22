#include "core/Application.h"

#include <iostream>

namespace Jade {
    Application::Application() {
        m_LayerInsertIndex = 0;
        m_Running = false;
        m_Layers = std::vector<Layer*>();
    }

    Application::~Application() {

    }

    void Application::Run() {
        while (m_Running) {
            float time = Window::GetTime();
            float dt = m_LastFrameTime - time;
            m_LastFrameTime = time;

            for (Layer* layer : m_Layers) {
                layer->OnUpdate(dt);
            }

            // ImGuiLayer->Begin();
            for (Layer* layer : m_Layers) {
                layer->OnImGuiRender();
            }
            // ImGuiLayer->End();

            //m_Window->OnUpdate();
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
}