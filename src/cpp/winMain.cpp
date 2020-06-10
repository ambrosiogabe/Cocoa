#include "jade/platform/windows/Window.h"
#include "jade/scenes/TestScene.h"
#include "jade/util/Log.h"

#include <windows.h>
#include <gl/GL.h>
#include "gl/glext.h"
#include "gl/wglext.h"

#include "build/build.h"

#define LAST(k,n) ((k) & ((1<<(n))-1))
#define MID(k,m,n) LAST((k)>>(m),((n)-(m)))

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;

    switch(message) {
        case WM_KEYUP:
        case WM_KEYDOWN: {
            static bool modifierIsPressed = 0;
            static int modifierKey = 0;

            int repeatCount = MID(lParam, 0, 16);
            int wScanCode = MID(lParam, 16, 24);
            int wExtendedKey = MID(lParam, 24, 25);
            int wContextCode = MID(lParam, 29, 30);
            int wPrevState = MID(lParam, 30, 31);
            int wTransitionState = MID(lParam, 31, 32); // 1 for keyup -- 0 for keydown

            int action = wTransitionState == 1 ? JADE_RELEASE : JADE_PRESS;
            if (action == JADE_PRESS && wPrevState == 1) {
                action = JADE_REPEAT;
            }

            //Log::Info("Repeat: %d  scanCode: %d  extendedKey: %d  contextCode: %d  prevState: %d  transitionState: %d", 
            //        repeatCount, wScanCode, wExtendedKey, wContextCode, wPrevState, wTransitionState);
            switch (wParam) {
                case (VK_ESCAPE): {
                    Window::KeyCallback(JADE_KEY_ESCAPE, wScanCode, action, modifierKey);
                    break;
                }
                case (VK_SPACE): {
                    Window::KeyCallback(JADE_KEY_SPACE, wScanCode, action, modifierKey);
                    break;
                }
                case (VK_BACK): {
                    Window::KeyCallback(JADE_KEY_BACKSPACE, wScanCode, action, modifierKey);
                    break;
                }
                case (VK_TAB): {
                    Window::KeyCallback(JADE_KEY_TAB, wScanCode, action, modifierKey);
                    break;
                }
                case (VK_CLEAR): {
                    Log::Assert(false, "Pressed unkown key 'clear'");
                    break;
                }
                case (VK_RETURN): {
                    Window::KeyCallback(JADE_KEY_ENTER, wScanCode, action, modifierKey);
                    break;
                }
                case (VK_LSHIFT): {
                    modifierIsPressed = action == JADE_PRESS;
                    modifierKey = JADE_MOD_SHIFT;
                    Window::KeyCallback(JADE_KEY_LEFT_SHIFT, wScanCode, action, modifierKey);
                    break;
                }
                case (VK_RSHIFT): {
                    modifierIsPressed = action == JADE_PRESS;
                    modifierKey = JADE_MOD_SHIFT;
                    Window::KeyCallback(JADE_KEY_RIGHT_SHIFT, wScanCode, action, modifierKey);
                    break;
                }
                case (VK_LCONTROL): {
                    modifierIsPressed = action == JADE_PRESS;
                    modifierKey = JADE_MOD_CONTROL;
                    Window::KeyCallback(JADE_KEY_LEFT_CONTROL, wScanCode, action, modifierKey);
                    break;
                }
                case (VK_RCONTROL): {
                    modifierIsPressed = action == JADE_PRESS;
                    modifierKey = JADE_MOD_CONTROL;
                    Window::KeyCallback(JADE_KEY_RIGHT_CONTROL, wScanCode, action, modifierKey);
                    break;
                }
                case (VK_LMENU): {
                    modifierIsPressed = action == JADE_PRESS;
                    modifierKey = JADE_MOD_ALT;
                    Window::KeyCallback(JADE_KEY_LEFT_ALT, wScanCode, action, modifierKey);
                    break;
                }
                case (VK_RMENU): {
                    modifierIsPressed = action == JADE_PRESS;
                    modifierKey = JADE_MOD_ALT;
                    Window::KeyCallback(JADE_KEY_RIGHT_ALT, wScanCode, action, modifierKey);
                    break;
                }
                case (VK_PAUSE): {
                    Window::KeyCallback(JADE_KEY_PAUSE, wScanCode, action, modifierKey);
                    break;
                } case (VK_CAPITAL): {
                    modifierIsPressed = action == JADE_PRESS;
                    modifierKey = JADE_MOD_CAPS_LOCK;
                    Window::KeyCallback(JADE_KEY_CAPS_LOCK, wScanCode, action, modifierKey);
                    break;
                } case (VK_PRIOR): {
                    Window::KeyCallback(JADE_KEY_PAGE_UP, wScanCode, action, modifierKey);
                    break;
                } case (VK_NEXT): {
                    Window::KeyCallback(JADE_KEY_PAGE_DOWN, wScanCode, action, modifierKey);
                    break;
                } case (VK_END): {
                    Window::KeyCallback(JADE_KEY_END, wScanCode, action, modifierKey);
                    break;
                } case (VK_HOME): {
                    Window::KeyCallback(JADE_KEY_HOME, wScanCode, action, modifierKey);
                    break;
                } case (VK_LEFT): {
                    Window::KeyCallback(JADE_KEY_LEFT, wScanCode, action, modifierKey);
                    break;
                } case (VK_UP): {
                    Window::KeyCallback(JADE_KEY_UP, wScanCode, action, modifierKey);
                    break;
                } case (VK_RIGHT): {
                    Window::KeyCallback(JADE_KEY_RIGHT, wScanCode, action, modifierKey);
                    break;
                } case (VK_DOWN): {
                    Window::KeyCallback(JADE_KEY_DOWN, wScanCode, action, modifierKey);
                    break;
                } case (VK_SNAPSHOT): {
                    Window::KeyCallback(JADE_KEY_PRINT_SCREEN, wScanCode, action, modifierKey);
                    break;
                } case (VK_INSERT): {
                    Window::KeyCallback(JADE_KEY_INSERT, wScanCode, action, modifierKey);
                    break;
                } case (VK_DELETE): {
                    Window::KeyCallback(JADE_KEY_DELETE, wScanCode, action, modifierKey);
                    break;
                } case (0x30): {
                    Window::KeyCallback(JADE_KEY_0, wScanCode, action, modifierKey);
                    break;
                } case (0x31): {
                    Window::KeyCallback(JADE_KEY_1, wScanCode, action, modifierKey);
                    break;
                } case (0x32): {
                    Window::KeyCallback(JADE_KEY_2, wScanCode, action, modifierKey);
                    break;
                } case (0x33): {
                    Window::KeyCallback(JADE_KEY_3, wScanCode, action, modifierKey);
                    break;
                } case (0x34): {
                    Window::KeyCallback(JADE_KEY_4, wScanCode, action, modifierKey);
                    break;
                } case (0x35): {
                    Window::KeyCallback(JADE_KEY_5, wScanCode, action, modifierKey);
                    break;
                } case (0x36): {
                    Window::KeyCallback(JADE_KEY_6, wScanCode, action, modifierKey);
                    break;
                } case (0x37): {
                    Window::KeyCallback(JADE_KEY_7, wScanCode, action, modifierKey);
                    break;
                } case (0x38): {
                    Window::KeyCallback(JADE_KEY_8, wScanCode, action, modifierKey);
                    break;
                } case (0x39): {
                    Window::KeyCallback(JADE_KEY_9, wScanCode, action, modifierKey);
                    break;
                } 
                case('A'):
                case('B'):
                case('C'):
                case('D'):
                case('E'):
                case('F'):
                case('G'):
                case('H'):
                case('I'):
                case('J'):
                case('K'):
                case('L'):
                case('M'):
                case('N'):
                case('O'):
                case('P'):
                case('Q'):
                case('R'):
                case('S'):
                case('T'):
                case('U'):
                case('V'):
                case('W'):
                case('X'):
                case('Y'):
                case('Z'): {
                    Window::KeyCallback(wParam, wScanCode, action, modifierKey);
                    break;
                }
            }

            break;
        }
        case WM_CLOSE: {
            PostQuitMessage(0);
            break;
        }
        default: {
            result = DefWindowProcA(hWnd, message, wParam, lParam);
        }
    }

    return result;
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

    if (Window::Create(hInstance, nCmdShow) != 0) {
        PostQuitMessage(1);
    }
    Window* window = Window::GetWindow();
    
    TestScene scene = TestScene();
    window->ChangeScene(&scene);

    MSG msg;
    while (window->IsRunning()) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                window->Stop();
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        window->Update(1.0f/60.0f);
        window->Render();
    }
    system("pause");
    window->Destroy();

    return msg.wParam;
}