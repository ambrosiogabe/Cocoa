#include "jade/core/Application.h"
#include "jade/renderer/DebugDraw.h"

#include <string>
#include <iostream>
#include <gl/gl3w.h>

namespace Jade {
    Application::Application() {
        m_LayerInsertIndex = 0;
        m_Running = true;
        m_Layers = std::vector<Layer*>();

        std::string title = std::string("Test Window");
        m_Window = JWindow::Create(1920, 1080, title);
        s_Instance = this;

        m_Framebuffer = new Framebuffer(3840, 2160);

        m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
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

            glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer->GetId());
            glViewport(0, 0, 3840, 2160);
            glClearColor(0.45f, 0.55f, 0.6f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            DebugDraw::BeginFrame();

            for (Layer* layer : m_Layers) {
                layer->OnUpdate(dt);
                layer->OnRender();
            }

            DebugDraw::EndFrame();
            m_CurrentScene->Render();

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glViewport(0, 0, m_Window->GetWidth(), m_Window->GetHeight());
            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT);

            m_CurrentScene->ImGui();
            // for (Layer* layer : m_Layers) {
            //     layer->OnImGuiRender();
            // }

            m_Window->OnUpdate();
            m_Window->Render();
        }

        for (Layer* layer : m_Layers) {
            layer->OnDetach();
        }
    }

    bool Application::OnWindowClose(WindowCloseEvent& e) {
        m_Running = false;
        m_Window->Destroy();
        return true;
    }

    void Application::OnEvent(Event& e) {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));

        for (auto it = m_Layers.end(); it != m_Layers.begin();) {
            (*--it)->OnEvent(e);
            if (e.Handled()) {
                break;
            }
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

    JWindow* Application::GetWindow() const {
        return m_Window;
    }

    Application* Application::s_Instance = nullptr;
    Application* Application::Get() {
        Log::Assert(s_Instance != nullptr, "Cannot get application. It is nullptr.");
        return s_Instance;
    }
}