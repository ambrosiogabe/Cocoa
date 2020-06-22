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

        virtual void Close() = 0;
        virtual void Hide() = 0;
        virtual void Show() = 0;

        virtual void Destroy() = 0;
        virtual void PollEvents() = 0;

        static float GetTime();

        // DEPRECATED
        static void KeyCallback(int key, int scancode, int action, int mods) {
            Window* win = Get();
            for (int i=0; i < 3; i++) {
                if (win->keyCallbacks[i] != nullptr) {
                    win->keyCallbacks[i](key, scancode, action, mods);
                }
            }
        }

        // DEPRECATED
        static void CursorCallback(double xpos, double ypos) {
            Window* win = Get();
            for (int i=0; i < 3; i++) {
                if (win->cursorCallbacks[i] != nullptr) {
                    win->cursorCallbacks[i](xpos, ypos);
                }
            }
        }

        // DEPRECATED
        static void MouseButtonCallback(int button, int action, int mods) {
            Window* win = Get();
            for (int i=0; i < 3; i++) {
                if (win->mouseButtonCallbacks[i] != nullptr) {
                    win->mouseButtonCallbacks[i](button, action, mods);
                }
            }
        }

        // DEPRECATED
        static void ScrollCallback(double xoffset, double yoffset) {
            Window* win = Get();
            for (int i=0; i < 3; i++) {
                if (win->scrollCallbacks[i] != nullptr) {
                    win->scrollCallbacks[i](xoffset, yoffset);
                }
            }
        }

        // DEPRECATED
        static void ResizeCallback(int width, int height) {
            Window* win = Get();
            for (int i=0; i < 3; i++) {
                if (win->resizeCallbacks[i] != nullptr) {
                    win->resizeCallbacks[i](width, height);
                }
            }
        }

        // DEPRECATED
        static void RegisterKeyCallback(KeyCallbackFnPt callback) {
            Window* win = Get();
            bool assigned = false;
            for (int i=0; i < 3; i++) {
                if (win->keyCallbacks[i] == nullptr) {
                    win->keyCallbacks[i] = callback;
                    assigned = true;
                    break;
                }
            }

            if (!assigned) {
                Log::Warning("Cannot register callback. Maximum number of key callbacks is 3.");
            }
        }

        // DEPRECATED
        static void RegisterCursorCallback(CursorCallbackFnPt callback) {
            Window* win = Get();
            bool assigned = false;
            for (int i=0; i < 3; i++) {
                if (win->cursorCallbacks[i] == nullptr) {
                    win->cursorCallbacks[i] = callback;
                    assigned = true;
                    break;
                }
            }

            if (!assigned) {
                Log::Warning("Cannot register callback. Maximum number of key callbacks is 3.");
            }
        }

        // DEPRECATED
        static void RegisterMouseButtonCallback(MouseButtonCallbackFnPt callback) {
            Window* win = Get();
            bool assigned = false;
            for (int i=0; i < 3; i++) {
                if (win->mouseButtonCallbacks[i] == nullptr) {
                    win->mouseButtonCallbacks[i] = callback;
                    assigned = true;
                    break;
                }
            }

            if (!assigned) {
                Log::Warning("Cannot register callback. Maximum number of key callbacks is 3.");
            }
        }

        // DEPRECATED
        static void RegisterScrollCallback(ScrollCallbackFnPt callback) {
            Window* win = Get();
            bool assigned = false;
            for (int i=0; i < 3; i++) {
                if (win->scrollCallbacks[i] == nullptr) {
                    win->scrollCallbacks[i] = callback;
                    assigned = true;
                    break;
                }
            }

            if (!assigned) {
                Log::Warning("Cannot register callback. Maximum number of key callbacks is 3.");
            }
        }

        // DEPRECATED
        static void RegisterResizeCallback(ResizeCallbackFnPt callback) {
            Window* win = Get();
            bool assigned = false;
            for (int i=0; i < 3; i++) {
                if (win->resizeCallbacks[i] == nullptr) {
                    win->resizeCallbacks[i] = callback;
                    assigned = true;
                    break;
                }
            }

            if (!assigned) {
                Log::Warning("Cannot register callback. Maximum number of key callbacks is 3.");
            }
        }

        // DEPRECATED -----------------------------------------------------------------
        static bool IsRunning() { return Window::Get()->_IsRunning(); }

        static void SetWidth(int newWidth) { Window::Get()->m_Width = newWidth; }
        static void SetHeight(int newHeight) { Window::Get()->m_Height = newHeight; }

        static int GetWidth() { return Window::Get()->m_Width; }
        static int GetHeight() { return Window::Get()->m_Height; }
        static float GetTargetAspectRatio() { return Window::Get()->m_TargetAspectRatio; }
        static Framebuffer* GetFramebuffer() { return Window::Get()->m_Framebuffer; }
        static Scene* GetScene() { return Window::Get()->m_CurrentScene; }
        static void* GetWindowHandle() { return Window::Get()->m_WindowHandle; }
        // DEPRECATED -----------------------------------------------------------------


        // DEPRECATED
        static void Update(Window* window, float dt) { 
            DebugDraw::BeginFrame();
            window->m_CurrentScene->Update(dt); 
        }

        // DEPRECATED
        static void Render() { Window::Get()->_Render(); }

        // DEPRECATED
        static void ChangeScene(Scene* newScene) {
            Window* win = Window::Get();
            win->m_CurrentScene = newScene;
            win->m_CurrentScene->Init();
            win->m_CurrentScene->Start();
        }

        static Window* Get();

    private:
        // DEPRECATED
        // void Init(bool tmp=false) {
        //     for (int i=0; i < 3; i++) {
        //         keyCallbacks[i] = nullptr;
        //         mouseButtonCallbacks[i] = nullptr;
        //         scrollCallbacks[i] = nullptr;
        //         cursorCallbacks[i] = nullptr;
        //         resizeCallbacks[i] = nullptr;
        //     }
        // }

    protected:
        virtual Window* _CreateWindow(int width, int height, const char* title) = 0;

        virtual void _Render() = 0;

        const bool _IsRunning() { return m_Running; }

    protected:
        static std::chrono::steady_clock::time_point m_TimeStarted;

        bool m_InitFocused = false;
        bool m_InitIconified = false;
        bool m_InitResizable = true;
        bool m_InitVisible = true;
        bool m_InitDecorated = true;
        bool m_InitAutoIconify = false;
        bool m_InitFloating = false;
        bool m_InitMaximized = false;
        bool m_InitCenterCursor = false;
        bool m_InitTransparentFramebuffer = false;
        bool m_InitHovered = false;
        bool m_InitFocusOnShow = true;
        bool m_InitAllocConsole = false;

        static Window* m_Instance;
        void* m_WindowHandle;

        // Registered callbacks
        KeyCallbackFnPt           keyCallbacks[3];
        CursorCallbackFnPt        cursorCallbacks[3];
        MouseButtonCallbackFnPt   mouseButtonCallbacks[3];
        ScrollCallbackFnPt        scrollCallbacks[3];
        ResizeCallbackFnPt        resizeCallbacks[3];

        Scene* m_CurrentScene;
        Framebuffer* m_Framebuffer;

        bool m_Running = true;
        int m_Width = 1920;
        int m_Height = 1080;
        float m_TargetAspectRatio = 3840.0f / 2160.0f;
    };
}

#ifdef _JADE_PLATFORM_WINDOWS
// Put all different implementation stubs in here, so that we can get rid
// of circular dependencies
#include "platform/windows/Win32Window.h"
#endif
