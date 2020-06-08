#pragma once

#include "jade/scenes/Scene.h"
#include <windows.h>

class Window {
public:
    int Create(HINSTANCE hInstance, int nCmdShow);
    void ShowMessage(LPCSTR message);
    void Render();
    void Destroy();

    static void Hide() { Window::GetWindow()->_Hide(); }
    static void Close() { Window::GetWindow()->_Close(); }
    static void Stop() { Window::GetWindow()->_Stop(); }
    static bool IsRunning() { return Window::GetWindow()->_IsRunning(); }

    void Update(float dt);
    static void ChangeScene(Scene* newScene);
    static Window* GetWindow();

private:
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
};