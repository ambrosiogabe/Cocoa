#pragma once

#include "core/Core.h"
#include "scenes/Scene.h"
#include "util/Log.h"
#include "core/ImGuiLayer.h"
#include "renderer/Framebuffer.h"
#include "renderer/DebugDraw.h"

#include <chrono>

#ifdef CreateWindow
#undef CreateWindow
#endif

namespace Jade {
    class Window {
    public:
        static int Init();
        static void InitHint(int hint, int value);
        static Window* CreateWindow(int width, int height, const char* title);
        
        static void Show(Window* window);

        virtual void Hide() = 0;
        virtual void Show() = 0;

        virtual void Destroy() = 0;
        virtual void PollEvents() = 0;
        virtual void SwapBuffers() = 0;
        virtual void* GetWindowHandle() = 0;

        static float GetTime();
        static void SetWindowUserPointer(Window* window, void* pointer);
        static void* GetWindowUserPointer(Window* window);
        static void* GetWindowHandle(Window* window);
        static void SwapInterval(int interval);

        static void SetWindowSizeCallback(Window* window, ResizeCallbackFnPt resizeCallback);
        static void SetKeyCallback(Window* window, KeyCallbackFnPt keyCallback);
        static void SetMouseButtonCallback(Window* window, MouseButtonCallbackFnPt mouseCallback);
        static void SetCursorPosCallback(Window* window, CursorCallbackFnPt cursorCallback);
        static void SetScrollCallback(Window* window, ScrollCallbackFnPt scrollCallback);
        static void SetWindowCloseCallback(Window* window, WindowCloseCallbackFnPt closeCallback);

    protected:
        virtual void UpdateSwapInterval() = 0;

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

        void* m_WindowUserPointer = nullptr;
        void* m_WindowHandle = nullptr;

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
