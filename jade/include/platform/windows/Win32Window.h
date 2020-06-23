#pragma once

#include "platform/Window.h"

#include <gl/GL.h>
#include <gl/glext.h>
#include <gl/wglext.h>

namespace Jade {
    class Win32Window : public Window {
    public:
        Win32Window() {}
        virtual void Destroy() override;
        virtual void PollEvents() override;
        virtual void SwapBuffers() override;

        static void ShowMessage(LPCSTR message);
        static void InitWin32(HINSTANCE hInstance);
        static void InitWin32();

        virtual void Show() override;
        virtual void Hide() override;

        static void DefaultResizeCallback(HWND window, int width, int height);
        static void DefaultMouseButtonCallback(HWND window, int button, int action, int mods);
        static void DefaultCursorCallback(HWND window, double xpos, double ypos);
        static void DefaultScrollCalback(HWND window, double xoffset, double yoffset);
        static void DefaultKeyCallback(HWND window, int key, int scancode, int action, int mods);
        static void DefaultCloseCallback(HWND window);

        static Window* CreateWindow(int width, int height, const char* title);

    protected:
        virtual void UpdateSwapInterval() override;

    private:
        static bool m_Initialized;

        static HINSTANCE m_HINSTANCE;
        HGLRC RC;
        HDC DC;
        HWND WND;

        static PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
    };
}