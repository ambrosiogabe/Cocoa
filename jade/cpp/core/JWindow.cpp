#include "core/JWindow.h"
#include "platform/Window.h"

namespace Jade {

    JWindow* JWindow::Create(uint32 width, uint32 height, const std::string& name) {
        JWindow* instance = Get();
        instance->m_WindowHandle = Window::CreateWindow(width, height, name.c_str());   
        return instance; 
    }

    void JWindow::OnUpdate() {
        m_WindowHandle->PollEvents();
    }

    void JWindow::Render() { 
        m_WindowHandle->SwapBuffers();
    }

    // Window Attributes
    void JWindow::SetVSync(bool enabled) {
        Log::Warning("SetVSync has not been implemented yet.");
    }

    bool JWindow::IsVSync() const {
        Log::Warning("IsVSync has not been implemented yet.");
        return false;
    }

    void* JWindow::GetNativeWindow() const {
        return (void*)m_WindowHandle;
    }

    bool JWindow::IsRunning() { 
        return m_Running; 
    }

    void JWindow::SetWidth(int newWidth) { 
        m_Width = newWidth; 
    }

    void JWindow::SetHeight(int newHeight) { 
        m_Height = newHeight; 
    }

    int JWindow::GetWidth() { 
        return m_Width; 
    }

    int JWindow::GetHeight() { 
        return m_Height; 
    }
    
    float JWindow::GetTargetAspectRatio() { 
        return m_TargetAspectRatio;
    }

    Framebuffer* JWindow::GetFramebuffer() { 
        return m_Framebuffer; 
    }

    Scene* JWindow::GetScene() { 
        return m_CurrentScene; 
    }

    void JWindow::ChangeScene(Scene* newScene) {
        JWindow* win = JWindow::Get();
        win->m_CurrentScene = newScene;
        win->m_CurrentScene->Init();
        win->m_CurrentScene->Start();
    }

    JWindow* JWindow::m_Instance = nullptr;
    JWindow* JWindow::Get() {
        if (JWindow::m_Instance == nullptr) {
            JWindow::m_Instance = new JWindow();
        }

        return JWindow::m_Instance;
    }
}