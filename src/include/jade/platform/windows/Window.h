#pragma once

#include "jade/scenes/Scene.h"
#include "jade/Jade.h"
#include "jade/util/Log.h"
#include "jade/events/Input.h"

#include <windows.h>

class Window {
public:
    static int Create(HINSTANCE hInstance, int nCmdShow);
    static void ShowMessage(LPCSTR message);
    void Render();
    void Destroy();

    static void KeyCallback(int key, int scancode, int action, int mods) {
        Window* win = GetWindow();
        for (int i=0; i < 3; i++) {
            if (win->keyCallbacks[i] != nullptr) {
                win->keyCallbacks[i](key, scancode, action, mods);
            }
        }
    }

    static void CursorCallback(double xpos, double ypos) {
        Window* win = GetWindow();
        for (int i=0; i < 3; i++) {
            if (win->cursorCallbacks[i] != nullptr) {
                win->cursorCallbacks[i](xpos, ypos);
            }
        }
    }

    static void MouseButtonCallback(int button, int action, int mods) {
        Window* win = GetWindow();
        for (int i=0; i < 3; i++) {
            if (win->mouseButtonCallbacks[i] != nullptr) {
                win->mouseButtonCallbacks[i](button, action, mods);
            }
        }
    }

    static void ScrollCallback(double xoffset, double yoffset) {
        Window* win = GetWindow();
        for (int i=0; i < 3; i++) {
            if (win->scrollCallbacks[i] != nullptr) {
                win->scrollCallbacks[i](xoffset, yoffset);
            }
        }
    }

    static void RegisterKeyCallback(KeyCallbackFnPt callback) {
        Window* win = GetWindow();
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
    static void RegisterCursorCallback(CursorCallbackFnPt callback);
    static void RegisterMouseButtonCallback(MouseButtonCallbackFnPt callback);
    static void RegisterScrollCallback(ScrollCallbackFnPt callback);

    static void Hide() { Window::GetWindow()->_Hide(); }
    static void Close() { Window::GetWindow()->_Close(); }
    static void Stop() { Window::GetWindow()->_Stop(); }
    static bool IsRunning() { return Window::GetWindow()->_IsRunning(); }

    void Update(float dt);
    static void ChangeScene(Scene* newScene);
    static Window* GetWindow();

private:
    Window::Window() {
        for (int i=0; i < 3; i++) {
            keyCallbacks[i] = nullptr;
            mouseButtonCallbacks[i] = nullptr;
            scrollCallbacks[i] = nullptr;
            cursorCallbacks[i] = nullptr;
        }
    }

    void _Hide();
    void _Close();
    void _Stop();
    const bool _IsRunning() { return m_Running; }

private:
    HGLRC RC;
    HDC DC;
    HWND WND;

    Scene* m_CurrentScene;
    bool m_Running;

    static Window* m_Instance;

    // Registered callbacks
    KeyCallbackFnPt           keyCallbacks[3];
    CursorCallbackFnPt        cursorCallbacks[3];
    MouseButtonCallbackFnPt   mouseButtonCallbacks[3];
    ScrollCallbackFnPt        scrollCallbacks[3];
};