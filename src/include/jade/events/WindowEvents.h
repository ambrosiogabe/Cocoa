#pragma once

#include "jade/platform/JWindow.h"
#include "jade/platform/windows/GlFunctions.h"

class WindowEvents {
public:
    static void ResizeCallback(int width, int height) {
        JWindow* win = Get()->m_Window;
        win->SetWidth(width);
        win->SetHeight(height);

        glViewport(0, 0, width, height);
    }

    static void Init(JWindow* window) {
        WindowEvents* winEvents = Get();
        winEvents->m_Window = window;
    }

    static WindowEvents* Get() {
        if (WindowEvents::m_Instance == nullptr) {
            WindowEvents::m_Instance = new WindowEvents();
        }

        return WindowEvents::m_Instance;
    }

private:
    WindowEvents() {}

private:
    static WindowEvents* m_Instance;

    JWindow* m_Window;
};

WindowEvents* WindowEvents::m_Instance = nullptr;