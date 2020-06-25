#pragma once

#include "util/Log.h"
#include "core/Application.h"
#include "platform/Window.h"
#include "events/Input.h"
#include "core/Core.h"

#include <windows.h>
#include <windowsx.h>
#include <gl/GL.h>
#include "gl/glext.h"
#include "gl/wglext.h"

#define LAST(k,n) ((k) & ((1<<(n))-1))
#define MID(k,m,n) LAST((k)>>(m),((n)-(m)))

//WindowUpdateFnPt Update = nullptr;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern Jade::Application* Jade::CreateApplication();


int main() {
    Jade::Application* application = Jade::CreateApplication();
    application->Run();
    delete application;

    system("pause");
    return 0;
}