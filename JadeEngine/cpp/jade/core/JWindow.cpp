#include "jade/core/JWindow.h"
#include "jade/events/WindowEvent.h"
#include "jade/events/KeyEvent.h"
#include "jade/events/MouseEvent.h"

namespace Jade {
    JWindow* JWindow::Create(uint32 width, uint32 height, const std::string& name) {
        return new JWindow(width, height, name); 
    }

    JWindow::JWindow(uint32 width, uint32 height, const std::string& name) {
        Init(width, height, name);
    }

    void JWindow::Init(uint32 width, uint32 height, const std::string& name) {
        Log::Assert(glfwInit(), "Unable to initialize GLFW");

        m_WindowHandle = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
        Log::Assert(m_WindowHandle != nullptr, "GLFW unable to create window.");

        glfwMakeContextCurrent(m_WindowHandle);
        SetVSync(true);

        Log::Assert(glewInit() == GLEW_OK, "Unable to initialize GLEW.");
        glfwSetWindowUserPointer(m_WindowHandle, this);

        // Set up event callbacks
        glfwSetWindowSizeCallback(m_WindowHandle, [](GLFWwindow* window, int width, int height) {
            JWindow* userWin = static_cast<JWindow*>(glfwGetWindowUserPointer(window));
            Log::Assert(userWin != nullptr, "JWindow is nullpointer in callback.");

            userWin->SetWidth(width);
            userWin->SetHeight(height);

            WindowResizeEvent e(width, height);
            userWin->m_EventCallback(e);
        });

        glfwSetWindowCloseCallback(m_WindowHandle, [](GLFWwindow* window) {
            JWindow* userWin = static_cast<JWindow*>(glfwGetWindowUserPointer(window));
            Log::Assert(userWin != nullptr, "JWindow is nullpointer in callback.");

            WindowCloseEvent e;
            userWin->m_EventCallback(e);
        });

        glfwSetKeyCallback(m_WindowHandle, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            JWindow* userWin = static_cast<JWindow*>(glfwGetWindowUserPointer(window));
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

        glfwSetCharCallback(m_WindowHandle, [](GLFWwindow* window, uint32 keycode) {
            JWindow* userWin = static_cast<JWindow*>(glfwGetWindowUserPointer(window));
            Log::Assert(userWin != nullptr, "JWindow is nullpointer in callback.");

            // Event& e = KeyTypedEvent(keycode);
            // userWin->EventCallback(e);
        });

        glfwSetMouseButtonCallback(m_WindowHandle, [](GLFWwindow* window, int button, int action, int mods) {
            JWindow* userWin = static_cast<JWindow*>(glfwGetWindowUserPointer(window));
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

        glfwSetScrollCallback(m_WindowHandle, [](GLFWwindow* window, double xoffset, double yoffset) {
            JWindow* userWin = static_cast<JWindow*>(glfwGetWindowUserPointer(window));
            Log::Assert(userWin != nullptr, "JWindow is nullpointer in callback.");

            MouseScrolledEvent e((float)xoffset, (float)yoffset);
            userWin->m_EventCallback(e);
        });

        glfwSetCursorPosCallback(m_WindowHandle, [](GLFWwindow* window, double xpos, double ypos) {
            JWindow* userWin = static_cast<JWindow*>(glfwGetWindowUserPointer(window));
            Log::Assert(userWin != nullptr, "JWindow is nullpointer in callback.");

            MouseMovedEvent e((float)xpos, (float)ypos);
            userWin->m_EventCallback(e);
        });
    }

    void JWindow::SetEventCallback(const EventCallbackFn& e) {
        m_EventCallback = e;
    }

    void JWindow::OnUpdate() {
        glfwPollEvents();
    }

    void JWindow::Render() { 
        glfwSwapBuffers(m_WindowHandle);
    }

    // Window Attributes
    void JWindow::SetVSync(bool enabled) {
        if (enabled) {
            glfwSwapInterval(1);
        } else {
            glfwSwapInterval(0);
        }

        m_VSync = enabled;
    }

    bool JWindow::IsVSync() const {
        return m_VSync;
    }

    GLFWwindow* JWindow::GetNativeWindow() const {
        return m_WindowHandle;
    }

    void JWindow::Destroy() {
        glfwDestroyWindow(m_WindowHandle);
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