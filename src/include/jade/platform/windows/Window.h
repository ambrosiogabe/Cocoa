#pragma once

#include "jade/scenes/Scene.h"
#include <windows.h>

class Window {
public:
    int Create(HINSTANCE hInstance, int nCmdShow);
    void ShowMessage(LPCSTR message);
    void Render();
    void Destroy();

    void Update(float dt);
    static void ChangeScene(Scene* newScene);
    static Window* GetWindow();

private:
    HGLRC RC;
    HDC DC;
    HWND WND;

    Scene* m_CurrentScene;

    static Window* m_Instance;
};