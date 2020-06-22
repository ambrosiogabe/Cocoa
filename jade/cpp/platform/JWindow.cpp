#include "platform/JWindow.h"
#include "core/Core.h"

JWindow* JWindow::m_Instance = nullptr;
JWindow* JWindow::Get() {
    if (JWindow::m_Instance == nullptr) {
#ifdef _JADE_PLATFORM_WINDOWS
        JWindow::m_Instance = new Win32Window();
        JWindow::m_Instance->Init();
#endif
    }

    return JWindow::m_Instance;
}

void JWindow::InitHint(int hint, int value) {
    JWindow* win = Get();
    switch(hint) {
        case JADE_WH_FOCUSED:
            win->m_InitFocused = value;
            break;
        case JADE_WH_ICONIFIED:
            win->m_InitIconified = value;
            break;
        case JADE_WH_RESIZABLE:
            win->m_InitResizable = value;
            break;
        case JADE_WH_VISIBLE:
            win->m_InitVisible = value;
            break;
        case JADE_WH_DECORATED:
            win->m_InitDecorated = value;
            break;
        case JADE_WH_AUTO_ICONIFY:
            win->m_InitAutoIconify = value;
            break;
        case JADE_WH_FLOATING:
            win->m_InitFloating = value;
            break;
        case JADE_WH_MAXIMIZED:
            win->m_InitMaximized = value;
            break;
        case JADE_WH_CENTER_CURSOR:
            win->m_InitCenterCursor = value;
            break;
        case JADE_WH_TRANSPARENT_FRAMEBUFFER:
            win->m_InitTransparentFramebuffer = value;
            break;
        case JADE_WH_HOVERED:
            win->m_InitHovered = value;
            break;
        case JADE_WH_FOCUS_ON_SHOW:
            win->m_InitFocusOnShow = value;
            break;
        case JADE_WH_ALLOC_CONSOLE:
            win->m_InitAllocConsole = value;
            break;
        default:
            Log::Warning("Unknown window flag 0x%08x", hint);
            break;
    }
}

JWindow* JWindow::CreateWindow(int width, int height, const char* title) {
    return Get()->_CreateWindow(width, height, title);
}

void JWindow::ShowJWindow(JWindow* window) {
    window->Show();
}

int JWindow::Init() {
    m_TimeStarted = std::chrono::high_resolution_clock::now();

    JWindow* win = Get();
    for (int i=0; i < 3; i++) {
        win->keyCallbacks[i] = nullptr;
        win->mouseButtonCallbacks[i] = nullptr;
        win->scrollCallbacks[i] = nullptr;
        win->cursorCallbacks[i] = nullptr;
        win->resizeCallbacks[i] = nullptr;
    }
    return 0;
}

float JWindow::GetTime() {
    auto time = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<float>(time - m_TimeStarted);
}