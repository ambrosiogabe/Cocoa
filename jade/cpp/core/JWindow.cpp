#include "core/JWindow.h"
#include "platform/Window.h"

namespace Jade {
    JWindow* JWindow::Create(uint32 width, uint32 height, const std::string& name) {
        return new JWindow(width, height, name); 
    }

    JWindow::JWindow(uint32 width, uint32 height, const std::string& name) {
        if (JWindow::m_Instance != nullptr) {
            Log::Assert(false, "Instance of JWindow already exists. Cannot have two singletons.");
        }
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

            // TODO: Send event
            // WindowResizeEvent e(width, height);
            // userWin->EventCallback(e);
        });

        Window::SetWindowCloseCallback(m_WindowHandle, [](Window* window) {
            JWindow* userWin = static_cast<JWindow*>(Window::GetWindowUserPointer(window));
            Log::Assert(userWin != nullptr, "JWindow is nullpointer in callback.");

            // TODO: send event
            // WindowCloseEvent e;
            // userWin->EventCallback(e);
        });

        Window::SetKeyCallback(m_WindowHandle, [](Window* window, int key, int scancode, int action, int mods) {
            JWindow* userWin = static_cast<JWindow*>(Window::GetWindowUserPointer(window));
            Log::Assert(userWin != nullptr, "JWindow is nullpointer in callback.");

            switch (action) {
                case JADE_PRESS: {
                    // TODO
                    // KeyPressedEvent e(key, 0);
                    // userWin->EventCallback(e);
                    break;
                }
                case JADE_RELEASE: {
                    // TODO
                    // KeyReleasedEvent e(key);
                    // userWin->EventCallback(e);
                    break;
                }
                case JADE_REPEAT: {
                    // KeyPressedEvent e(key, 1);
                    // userWin->EventCallback(e);
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
                    // MouseButtonPressedEvent e(button);
                    // userWin->EventCallback(e);
                    break;
                }
                case JADE_RELEASE: {
                    // MouseButtonReleasedEvent e(button);
                    // userWin->EventCallback(e);
                    break;
                }
            }
        }); 

        Window::SetScrollCallback(m_WindowHandle, [](Window* window, double xoffset, double yoffset) {
            JWindow* userWin = static_cast<JWindow*>(Window::GetWindowUserPointer(window));
            Log::Assert(userWin != nullptr, "JWindow is nullpointer in callback.");

            // MouseScrolledEvent e((float)xoffset, (float)yoffset);
            // userWin->EventCallback(e);
        });

        Window::SetCursorPosCallback(m_WindowHandle, [](Window* window, double xpos, double ypos) {
            JWindow* userWin = static_cast<JWindow*>(Window::GetWindowUserPointer(window));
            Log::Assert(userWin != nullptr, "JWindow is nullpointer in callback.");

            // MouseMovedEvent e((float)xpos, (float)ypos);
            // userWin->EventCallback(e);
        });
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

    JWindow* JWindow::m_Instance = nullptr;
    JWindow* JWindow::Get() {
        if (JWindow::m_Instance == nullptr) {
            JWindow::m_Instance = new JWindow();
        }

        return JWindow::m_Instance;
    }
}