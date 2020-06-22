#include "platform/Window.h"
#include "core/Core.h"

namespace Jade {
    std::vector<Window*> Window::m_Windows = std::vector<Window*>();
    std::chrono::steady_clock::time_point Window::m_TimeStarted = std::chrono::high_resolution_clock::now();

    bool Window::m_InitFocused = false;
    bool Window::m_InitIconified = false;
    bool Window::m_InitResizable = true;
    bool Window::m_InitVisible = true;
    bool Window::m_InitDecorated = true;
    bool Window::m_InitAutoIconify = false;
    bool Window::m_InitFloating = false;
    bool Window::m_InitMaximized = false;
    bool Window::m_InitCenterCursor = false;
    bool Window::m_InitTransparentFramebuffer = false;
    bool Window::m_InitHovered = false;
    bool Window::m_InitFocusOnShow = true;
    bool Window::m_InitAllocConsole = false;

    void Window::InitHint(int hint, int value) {
        switch(hint) {
            case JADE_WH_FOCUSED:
                m_InitFocused = value;
                break;
            case JADE_WH_ICONIFIED:
                m_InitIconified = value;
                break;
            case JADE_WH_RESIZABLE:
                m_InitResizable = value;
                break;
            case JADE_WH_VISIBLE:
                m_InitVisible = value;
                break;
            case JADE_WH_DECORATED:
                m_InitDecorated = value;
                break;
            case JADE_WH_AUTO_ICONIFY:
                m_InitAutoIconify = value;
                break;
            case JADE_WH_FLOATING:
                m_InitFloating = value;
                break;
            case JADE_WH_MAXIMIZED:
                m_InitMaximized = value;
                break;
            case JADE_WH_CENTER_CURSOR:
                m_InitCenterCursor = value;
                break;
            case JADE_WH_TRANSPARENT_FRAMEBUFFER:
                m_InitTransparentFramebuffer = value;
                break;
            case JADE_WH_HOVERED:
                m_InitHovered = value;
                break;
            case JADE_WH_FOCUS_ON_SHOW:
                m_InitFocusOnShow = value;
                break;
            case JADE_WH_ALLOC_CONSOLE:
                m_InitAllocConsole = value;
                break;
            default:
                Log::Warning("Unknown window flag 0x%08x", hint);
                break;
        }
    }

    Window* Window::CreateWindow(int width, int height, const char* title) {
#ifdef _WIN32
        return Win32Window::CreateWindow(width, height, title);
#endif
    }

    void Window::Show(Window* window) {
        window->Show();
    }

    int Window::Init() {
        m_TimeStarted = std::chrono::high_resolution_clock::now();
        return 0;
    }

    float Window::GetTime() {
        auto time = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<float>(time - m_TimeStarted).count();
    }

    void Window::SetWindowSizeCallback(Window* window, ResizeCallbackFnPt resizeCallback) {
        window->m_ResizeCallback = resizeCallback;
    }

    void Window::SetKeyCallback(Window* window, KeyCallbackFnPt keyCallback) {
        window->m_KeyCallback = keyCallback;
    }

    void Window::SetMouseButtonCallback(Window* window, MouseButtonCallbackFnPt mouseCallback) {
        window->m_MouseButtonCallback = mouseCallback;
    }

    void Window::SetCursorPosCallback(Window* window, CursorCallbackFnPt cursorCallback) {
        window->m_CursorCallback = cursorCallback;
    }

    void Window::SetScrollCallback(Window* window, ScrollCallbackFnPt scrollCallback) {
        window->m_ScrollCallback = scrollCallback;
    }
}