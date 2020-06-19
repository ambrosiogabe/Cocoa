#include "platform/JWindow.h"

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