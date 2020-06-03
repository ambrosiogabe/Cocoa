#include "jade/platform/windows/Window.h"
#include "jade/scenes/TestScene.h"

#include <windows.h>
#include <gl/GL.h>
#include "gl/glext.h"
#include "gl/wglext.h"

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch(message) {
        case WM_KEYDOWN: {
            if (wParam == VK_ESCAPE) {
                PostQuitMessage(0);
            }
            break;
        }
        case WM_CLOSE: {
            PostQuitMessage(0);
            break;
        }
        default: {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    return 0;
}

ATOM registerClass(HINSTANCE hInstance) {
    WNDCLASSEX wcex;
    ZeroMemory(&wcex, sizeof(wcex));
    wcex.cbSize = sizeof(wcex);
    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wcex.lpfnWndProc = WindowProcedure;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.lpszClassName = "Core";

    return RegisterClassEx(&wcex);
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    registerClass(hInstance);

    Window window;
    if (window.Create(hInstance, nCmdShow) != 0) {
        PostQuitMessage(1);
    }
    
    TestScene scene = TestScene();
    window.ChangeScene(&scene);

    MSG msg;
    bool active = true;
    while (active) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                active = false;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        window.Update(1.0f/60.0f);
        window.Render();
    }
    window.Destroy();

    return msg.wParam;
}