#pragma once

#include "platform/Window.h"
#include "platform/windows/GlFunctions.h"

namespace Jade {
    class WindowEvents {
    public:
        static void ResizeCallback(int width, int height) {
            Window* win = Get()->m_Window;
            win->SetWidth(width);
            win->SetHeight(height);

            glViewport(0, 0, width, height);
        }

        static void Init(Window* window) {
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

        Window* m_Window;
    };

    WindowEvents* WindowEvents::m_Instance = nullptr;

}