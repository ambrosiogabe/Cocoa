#pragma once

#include "platform/Window.h"

#include <gl/GL.h>
#include <gl/glext.h>
#include <gl/wglext.h>
#include <vector>

#ifdef CreateWindow
#undef CreateWindow
#endif

namespace Jade {
    class Win32Window : public Window {
    public:
        friend class Window;

        Win32Window() {}

        static void ShowMessage(LPCSTR message);
        static void InitWin32(HINSTANCE hInstance);
        static void InitWin32();

        static void DefaultResizeCallback(HWND window, int width, int height);
        static void DefaultMouseButtonCallback(HWND window, int button, int action, int mods);
        static void DefaultCursorCallback(HWND window, double xpos, double ypos);
        static void DefaultScrollCalback(HWND window, double xoffset, double yoffset);
        static void DefaultKeyCallback(HWND window, int key, int scancode, int action, int mods);
        static void DefaultCloseCallback(HWND window);

        static Window* CreateWindow(int width, int height, const char* title);

    protected:
        static void _PollEvents();

        virtual void UpdateSwapInterval() override;
        virtual void _SetWindowIcon(int count, const WindowImage* images) override;
        virtual void _Destroy() override;
        virtual void _SwapBuffers() override;
        virtual void _MakeContextCurrent() override;
        virtual void* _GetWindowHandle() override;

        virtual void _SetWindowPos(int x, int y) override;
        virtual void _SetWindowSize(int width, int height) override;
        virtual void _SetWindowTitle(const char* title) override;

        virtual void _Show() override;
        virtual void _Hide() override;

    private:
        static bool m_Initialized;

        static HINSTANCE m_HINSTANCE;
        HGLRC RC;
        HDC DC;
        HWND WND;
        std::vector<HICON> m_Icons = std::vector<HICON>();

        static PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
    };
}