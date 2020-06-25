#include "core/JWindow.h"
#include "platform/Window.h"
#include "events/WindowEvent.h"
#include "events/KeyEvent.h"
#include "events/MouseEvent.h"

namespace Jade {
    JWindow* JWindow::Create(uint32 width, uint32 height, const std::string& name) {
        return new JWindow(width, height, name); 
    }

    JWindow::JWindow(uint32 width, uint32 height, const std::string& name) {
        Init(width, height, name);
    }

    void JWindow::Init(uint32 width, uint32 height, const std::string& name) {
        Window::Init();

        m_WindowHandle = Window::CreateWindow(width, height, name.c_str());
        SetVSync(true);

        Window::SetWindowUserPointer(m_WindowHandle, this);

        // Set up event callbacks
        Window::SetWindowSizeCallback(m_WindowHandle, [](Window* window, int width, int height) {
            JWindow* userWin = static_cast<JWindow*>(Window::GetWindowUserPointer(window));
            Log::Assert(userWin != nullptr, "JWindow is nullpointer in callback.");

            userWin->SetWidth(width);
            userWin->SetHeight(height);

            WindowResizeEvent e(width, height);
            userWin->m_EventCallback(e);
        });

        Window::SetWindowCloseCallback(m_WindowHandle, [](Window* window) {
            JWindow* userWin = static_cast<JWindow*>(Window::GetWindowUserPointer(window));
            Log::Assert(userWin != nullptr, "JWindow is nullpointer in callback.");

            WindowCloseEvent e;
            userWin->m_EventCallback(e);
        });

        Window::SetKeyCallback(m_WindowHandle, [](Window* window, int key, int scancode, int action, int mods) {
            JWindow* userWin = static_cast<JWindow*>(Window::GetWindowUserPointer(window));
            Log::Assert(userWin != nullptr, "JWindow is nullpointer in callback.");

            switch (action) {
                case JADE_PRESS: {
                    KeyPressedEvent e(key, 0);
                    userWin->m_EventCallback(e);
                    break;
                }
                case JADE_RELEASE: {
                    KeyReleasedEvent e(key);
                    userWin->m_EventCallback(e);
                    break;
                }
                case JADE_REPEAT: {
                    KeyPressedEvent e(key, 1);
                    userWin->m_EventCallback(e);
                    break;
                }
            }
        });

        // TODO Create char callback function
        // Window::SetCharCallback(m_WindowHandle, [](Window* window, uint32 keycode) {
        //     JWindow* userWin = static_cast<JWindow*>(Window::GetWindowUserPointer(window));
        //     Log::Assert(userWin != nullptr, "JWindow is nullpointer in callback.");

        //     Event& e = KeyTypedEvent(keycode);
        //     userWin->EventCallback(e);
        // });

        Window::SetMouseButtonCallback(m_WindowHandle, [](Window* window, int button, int action, int mods) {
            JWindow* userWin = static_cast<JWindow*>(Window::GetWindowUserPointer(window));
            Log::Assert(userWin != nullptr, "JWindow is nullpointer in callback.");

            switch (action) {
                case JADE_PRESS: {
                    MouseButtonPressedEvent e(button);
                    userWin->m_EventCallback(e);
                    break;
                }
                case JADE_RELEASE: {
                    MouseButtonReleasedEvent e(button);
                    userWin->m_EventCallback(e);
                    break;
                }
            }
        }); 

        Window::SetScrollCallback(m_WindowHandle, [](Window* window, double xoffset, double yoffset) {
            JWindow* userWin = static_cast<JWindow*>(Window::GetWindowUserPointer(window));
            Log::Assert(userWin != nullptr, "JWindow is nullpointer in callback.");

            MouseScrolledEvent e((float)xoffset, (float)yoffset);
            userWin->m_EventCallback(e);
        });

        Window::SetCursorPosCallback(m_WindowHandle, [](Window* window, double xpos, double ypos) {
            JWindow* userWin = static_cast<JWindow*>(Window::GetWindowUserPointer(window));
            Log::Assert(userWin != nullptr, "JWindow is nullpointer in callback.");

            MouseMovedEvent e((float)xpos, (float)ypos);
            userWin->m_EventCallback(e);
        });
    }

    void JWindow::SetEventCallback(const EventCallbackFn& e) {
        m_EventCallback = e;
    }

    void JWindow::OnUpdate() {
        m_WindowHandle->PollEvents();
    }

    void JWindow::Render() { 
        m_WindowHandle->SwapBuffers();
    }

    // Window Attributes
    void JWindow::SetVSync(bool enabled) {
        if (enabled) {
            Window::SwapInterval(1);
        } else {
            Window::SwapInterval(0);
        }

        m_VSync = enabled;
    }

    bool JWindow::IsVSync() const {
        return m_VSync;
    }

    void* JWindow::GetNativeWindow() const {
        return m_WindowHandle->GetWindowHandle();
    }

    void JWindow::Destroy() {
        m_WindowHandle->Destroy();
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
}