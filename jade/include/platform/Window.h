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
        // New window functions
        // to replace old functions
        static int Init();
        static void InitHint(int hint, int value);
        static Window* CreateWindow(int width, int height, const char* title);
        
        static void Show(Window* window);

        virtual void Hide() = 0;
        virtual void Show() = 0;

        virtual void Destroy() = 0;
        virtual void PollEvents() = 0;
        virtual void SwapBuffers() = 0;

        static float GetTime();

        static void SetWindowSizeCallback(Window* window, ResizeCallbackFnPt resizeCallback);
        static void SetKeyCallback(Window* window, KeyCallbackFnPt keyCallback);
        static void SetMouseButtonCallback(Window* window, MouseButtonCallbackFnPt mouseCallback);
        static void SetCursorPosCallback(Window* window, CursorCallbackFnPt cursorCallback);
        static void SetScrollCallback(Window* window, ScrollCallbackFnPt scrollCallback);

        // DEPRECATED -----------------------------------------------------------------
        // static bool IsRunning() { return Window::Get()->_IsRunning(); }

        // static void SetWidth(int newWidth) { Window::Get()->m_Width = newWidth; }
        // static void SetHeight(int newHeight) { Window::Get()->m_Height = newHeight; }

        // static int GetWidth() { return Window::Get()->m_Width; }
        // static int GetHeight() { return Window::Get()->m_Height; }
        // static float GetTargetAspectRatio() { return Window::Get()->m_TargetAspectRatio; }
        // static Framebuffer* GetFramebuffer() { return Window::Get()->m_Framebuffer; }
        // static Scene* GetScene() { return Window::Get()->m_CurrentScene; }
        // static void* GetWindowHandle() { return Window::Get()->m_WindowHandle; }
        // DEPRECATED -----------------------------------------------------------------


        // DEPRECATED
        // static void Update(Window* window, float dt) { 
        //     DebugDraw::BeginFrame();
        //     window->m_CurrentScene->Update(dt); 
        // }

        // // DEPRECATED
        // static void Render() { Window::Get()->_Render(); }

        // // DEPRECATED
        // static void ChangeScene(Scene* newScene) {
        //     Window* win = Window::Get();
        //     win->m_CurrentScene = newScene;
        //     win->m_CurrentScene->Init();
        //     win->m_CurrentScene->Start();
        // }

        // static Window* Get();

    public:
        // Callback function pointers
        KeyCallbackFnPt           m_KeyCallback = nullptr;
        CursorCallbackFnPt        m_CursorCallback = nullptr;
        MouseButtonCallbackFnPt   m_MouseButtonCallback = nullptr;
        ScrollCallbackFnPt        m_ScrollCallback = nullptr;
        ResizeCallbackFnPt        m_ResizeCallback = nullptr;

    protected:
        // virtual void _Render() = 0;

        // const bool _IsRunning() { return m_Running; }

    protected:
        static std::chrono::steady_clock::time_point m_TimeStarted;

        static bool m_InitFocused;
        static bool m_InitIconified;
        static bool m_InitResizable;
        static bool m_InitVisible;
        static bool m_InitDecorated;
        static bool m_InitAutoIconify;
        static bool m_InitFloating;
        static bool m_InitMaximized;
        static bool m_InitCenterCursor;
        static bool m_InitTransparentFramebuffer;
        static bool m_InitHovered;
        static bool m_InitFocusOnShow;
        static bool m_InitAllocConsole;

        static std::vector<Window*> m_Windows;

        // void* m_WindowHandle;

        // Scene* m_CurrentScene;
        // Framebuffer* m_Framebuffer;

        // bool m_Running = true;
        // int m_Width = 1920;
        // int m_Height = 1080;
        // float m_TargetAspectRatio = 3840.0f / 2160.0f;
    };
}

#ifdef _JADE_PLATFORM_WINDOWS
// Put all different implementation stubs in here, so that we can get rid
// of circular dependencies
#include "platform/windows/Win32Window.h"
#endif
