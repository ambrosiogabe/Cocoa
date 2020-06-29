// TODO: This code is now deprecated in favor of GLFW, may come back to it at some point
#if 0

#pragma once

#include "jade/core/Core.h"

#include <chrono>

#ifdef CreateWindow
#undef CreateWindow
#endif

namespace Jade {
    struct WindowImage {
        int m_Width;
        int m_Height;
        uint32* m_Pixels;
    };

    class Window {
    public:
        static int Init();
        static void WindowHint(int hint, int value);
        static Window* CreateWindow(int width, int height, const char* title);
        
        static void Show(Window* window);

        static float GetTime();

        static bool WindowShouldClose(Window* window);
        static void SetWindowShouldClose(Window* window, int value);

        static void SetWindowTitle(Window* window, const char* title);
        static void SetWindowIcon(Window* window, int count, const WindowImage* images);
        //static void SetWindowAspectRatio(Window* window, int numer, int denom);

        static void SetWindowPos(Window* window, int xpos, int ypos);
        static void GetWindowPos(Window* window, int* xpos, int* ypos);
        static void SetWindowSize(Window* window, int width, int height);
        static void GetWindowSize(Window* window, int* width, int* height);
        static void SetWindowSizeLimits(Window* window, int minwidth, int minheight, int maxwidth, int maxheight);

        static void SetWindowUserPointer(Window* window, void* pointer);
        static void* GetWindowUserPointer(Window* window);
        static void* GetWindowHandle(Window* window);
        static void SwapInterval(int interval);
        static void SwapBuffers(Window* window);
        static void MakeContextCurrent(Window* window);
        static void PollEvents();
        static void Destroy(Window* window);

        static void SetWindowSizeCallback(Window* window, ResizeCallbackFnPt resizeCallback);
        static void SetKeyCallback(Window* window, KeyCallbackFnPt keyCallback);
        static void SetMouseButtonCallback(Window* window, MouseButtonCallbackFnPt mouseCallback);
        static void SetCursorPosCallback(Window* window, CursorCallbackFnPt cursorCallback);
        static void SetScrollCallback(Window* window, ScrollCallbackFnPt scrollCallback);
        static void SetWindowCloseCallback(Window* window, WindowCloseCallbackFnPt closeCallback);

    protected:
        virtual void UpdateSwapInterval() = 0;
        virtual void _Hide() = 0;
        virtual void _Show() = 0;

        virtual void _Destroy() = 0;
        virtual void _SwapBuffers() = 0;
        virtual void _MakeContextCurrent() = 0;
        virtual void* _GetWindowHandle() = 0;
        virtual void _SetWindowIcon(int count, const WindowImage* images) = 0;

        virtual void _SetWindowPos(int xpos, int ypos) = 0;
        virtual void _SetWindowSize(int width, int height) = 0;
        virtual void _SetWindowTitle(const char* title) = 0;

    protected:
        static int s_SwapInterval;
        static std::chrono::steady_clock::time_point s_TimeStarted;

        static bool s_InitFocused;
        static bool s_InitIconified;
        static bool s_InitResizable;
        static bool s_InitVisible;
        static bool s_InitDecorated;
        static bool s_InitAutoIconify;
        static bool s_InitFloating;
        static bool s_InitMaximized;
        static bool s_InitCenterCursor;
        static bool s_InitTransparentFramebuffer;
        static bool s_InitHovered;
        static bool s_InitFocusOnShow;
        static bool s_InitAllocConsole;

        static std::vector<Window*> s_Windows;

        bool m_WindowShouldClose = false;
        void* m_WindowUserPointer = nullptr;
        void* m_WindowHandle = nullptr;

        int m_XPos = 0;
        int m_YPos = 0;
        int m_Width = 0;
        int m_Height = 0;

        // Callback function pointers
        KeyCallbackFnPt           m_KeyCallback = nullptr;
        CursorCallbackFnPt        m_CursorCallback = nullptr;
        MouseButtonCallbackFnPt   m_MouseButtonCallback = nullptr;
        ScrollCallbackFnPt        m_ScrollCallback = nullptr;
        ResizeCallbackFnPt        m_ResizeCallback = nullptr;
        WindowCloseCallbackFnPt   m_CloseCallback = nullptr;

        friend class Win32Window;
    };
}

#ifdef _JADE_PLATFORM_WINDOWS
// Put all different implementation stubs in here, so that we can get rid
// of circular dependencies
#include "platform/windows/Win32Window.h"
#endif

#endif