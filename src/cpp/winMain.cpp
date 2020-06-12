#include "jade/platform/windows/Window.h"
#include "jade/scenes/TestScene.h"
#include "jade/util/Log.h"

#include <windows.h>
#include <windowsx.h>
#include <gl/GL.h>
#include "gl/glext.h"
#include "gl/wglext.h"

#include "build/build.h"

#define LAST(k,n) ((k) & ((1<<(n))-1))
#define MID(k,m,n) LAST((k)>>(m),((n)-(m)))

//WindowUpdateFnPt Update = nullptr;

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;
    static bool modifierIsPressed = 0;
    static int modifierKey = 0;

    switch(message) {
        case WM_KEYUP:
        case WM_KEYDOWN: {
            //int repeatCount = MID(lParam, 0, 16);
            int wScanCode = MID(lParam, 16, 24);
            //int wExtendedKey = MID(lParam, 24, 25);
            //int wContextCode = MID(lParam, 29, 30);
            int wPrevState = MID(lParam, 30, 31);
            int wTransitionState = MID(lParam, 31, 32); // 1 for keyup -- 0 for keydown

            int action = wTransitionState == 1 ? JADE_RELEASE : JADE_PRESS;
            if (action == JADE_PRESS && wPrevState == 1) {
                action = JADE_REPEAT;
            }

            if (action == JADE_RELEASE && wParam == modifierKey) {
                modifierKey = 0;
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
                } case (VK_NUMPAD0): {
                    Window::KeyCallback(JADE_KEY_NUMPAD_0, wScanCode, action, modifierKey);
                    break;
                } case (VK_NUMPAD1): {
                    Window::KeyCallback(JADE_KEY_NUMPAD_1, wScanCode, action, modifierKey);
                    break;
                } case (VK_NUMPAD2): {
                    Window::KeyCallback(JADE_KEY_NUMPAD_2, wScanCode, action, modifierKey);
                    break;
                } case (VK_NUMPAD3): {
                    Window::KeyCallback(JADE_KEY_NUMPAD_3, wScanCode, action, modifierKey);
                    break;
                } case (VK_NUMPAD4): {
                    Window::KeyCallback(JADE_KEY_NUMPAD_4, wScanCode, action, modifierKey);
                    break;
                } case (VK_NUMPAD5): {
                    Window::KeyCallback(JADE_KEY_NUMPAD_5, wScanCode, action, modifierKey);
                    break;
                } case (VK_NUMPAD6): {
                    Window::KeyCallback(JADE_KEY_NUMPAD_6, wScanCode, action, modifierKey);
                    break;
                } case (VK_NUMPAD7): {
                    Window::KeyCallback(JADE_KEY_NUMPAD_7, wScanCode, action, modifierKey);
                    break;
                } case (VK_NUMPAD8): {
                    Window::KeyCallback(JADE_KEY_NUMPAD_8, wScanCode, action, modifierKey);
                    break;
                } case (VK_NUMPAD9): {
                    Window::KeyCallback(JADE_KEY_NUMPAD_9, wScanCode, action, modifierKey);
                    break;
                } case (VK_F1): {
                    Window::KeyCallback(JADE_KEY_F1, wScanCode, action, modifierKey);
                    break;
                } case (VK_F2): {
                    Window::KeyCallback(JADE_KEY_F2, wScanCode, action, modifierKey);
                    break;
                } case (VK_F3): {
                    Window::KeyCallback(JADE_KEY_F3, wScanCode, action, modifierKey);
                    break;
                } case (VK_F4): {
                    Window::KeyCallback(JADE_KEY_F4, wScanCode, action, modifierKey);
                    break;
                } case (VK_F5): {
                    Window::KeyCallback(JADE_KEY_F5, wScanCode, action, modifierKey);
                    break;
                } case (VK_F6): {
                    Window::KeyCallback(JADE_KEY_F6, wScanCode, action, modifierKey);
                    break;
                } case (VK_F7): {
                    Window::KeyCallback(JADE_KEY_F7, wScanCode, action, modifierKey);
                    break;
                } case (VK_F8): {
                    Window::KeyCallback(JADE_KEY_F8, wScanCode, action, modifierKey);
                    break;
                } case (VK_F9): {
                    Window::KeyCallback(JADE_KEY_F9, wScanCode, action, modifierKey);
                    break;
                } case (VK_F10): {
                    Window::KeyCallback(JADE_KEY_F10, wScanCode, action, modifierKey);
                    break;
                } case (VK_F11): {
                    Window::KeyCallback(JADE_KEY_F11, wScanCode, action, modifierKey);
                    break;
                } case (VK_F12): {
                    Window::KeyCallback(JADE_KEY_F12, wScanCode, action, modifierKey);
                    break;
                } case (VK_F13): {
                    Window::KeyCallback(JADE_KEY_F13, wScanCode, action, modifierKey);
                    break;
                } case (VK_F14): {
                    Window::KeyCallback(JADE_KEY_F14, wScanCode, action, modifierKey);
                    break;
                } case (VK_F15): {
                    Window::KeyCallback(JADE_KEY_F15, wScanCode, action, modifierKey);
                    break;
                } case (VK_F16): {
                    Window::KeyCallback(JADE_KEY_F16, wScanCode, action, modifierKey);
                    break;
                } case (VK_F17): {
                    Window::KeyCallback(JADE_KEY_F17, wScanCode, action, modifierKey);
                    break;
                } case (VK_F18): {
                    Window::KeyCallback(JADE_KEY_F18, wScanCode, action, modifierKey);
                    break;
                } case (VK_F19): {
                    Window::KeyCallback(JADE_KEY_F19, wScanCode, action, modifierKey);
                    break;
                } case (VK_F20): {
                    Window::KeyCallback(JADE_KEY_F20, wScanCode, action, modifierKey);
                    break;
                } case (VK_F21): {
                    Window::KeyCallback(JADE_KEY_F21, wScanCode, action, modifierKey);
                    break;
                } case (VK_F22): {
                    Window::KeyCallback(JADE_KEY_F22, wScanCode, action, modifierKey);
                    break;
                } case (VK_F23): {
                    Window::KeyCallback(JADE_KEY_F23, wScanCode, action, modifierKey);
                    break;
                } case (VK_F24): {
                    Window::KeyCallback(JADE_KEY_F24, wScanCode, action, modifierKey);
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
                    Window::KeyCallback((int)wParam, wScanCode, action, modifierKey);
                    break;
                }
            }

            break;
        }

        // Mouse button events
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN: {
            int button = wParam == MK_LBUTTON ? JADE_MOUSE_BUTTON_LEFT : wParam == MK_RBUTTON ? JADE_MOUSE_BUTTON_RIGHT : wParam == MK_MBUTTON ? JADE_MOUSE_BUTTON_MIDDLE : -1;
            if (button != -1) {
                Window::MouseButtonCallback(button, JADE_PRESS, modifierKey);
            }
            break;
        }
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDBLCLK: {
            int button = wParam == MK_LBUTTON ? JADE_MOUSE_BUTTON_LEFT : wParam == MK_RBUTTON ? JADE_MOUSE_BUTTON_RIGHT : wParam == MK_MBUTTON ? JADE_MOUSE_BUTTON_MIDDLE : -1;
            if (button != -1) {
                Window::MouseButtonCallback(button, JADE_DOUBLE_CLICK, modifierKey);
            }
            break;
        }
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP: {
            int button = message == WM_LBUTTONUP ? JADE_MOUSE_BUTTON_LEFT : message == WM_RBUTTONUP ? JADE_MOUSE_BUTTON_RIGHT : message == WM_MBUTTONUP ? JADE_MOUSE_BUTTON_MIDDLE : -1;
            if (button != -1) {
                Window::MouseButtonCallback(button, JADE_RELEASE, modifierKey);
            }
            break;
        }

        // Mouse move events
        case WM_MOUSEMOVE: {
            int xpos = GET_X_LPARAM(lParam);
            int ypos = GET_Y_LPARAM(lParam);
            Window::CursorCallback((double)xpos, (double)ypos);
            break;
        }

        // Mouse scroll events
        case WM_MOUSEWHEEL: {
            int wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
            // TODO: Find out if horizontal scroll is supported by windows
            Window::ScrollCallback(0.0, (double)wheelDelta);
            break;
        }

        // Window resize events
        case WM_SIZE: {
            RECT rect;
            GetWindowRect(hWnd, &rect);
            int newHeight = rect.bottom - rect.top;
            int newWidth = rect.right - rect.left;
            Window::ResizeCallback(newWidth, newHeight);
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

void LoadGameCode() {
    // TODO: MAKE THIS WORK SOMEHOW!!!
    //CopyFileA("jade.dll", "jade_temp.dll", FALSE);
    //HMODULE gameCodeDll = LoadLibraryA("jade_temp.dll");
    // if (gameCodeDll) {
    //     Update = (WindowUpdateFnPt)GetProcAddress(gameCodeDll, "WindowUpdate");
    // }
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    registerClass(hInstance);

    if (Window::Create(hInstance, nCmdShow) != 0) {
        PostQuitMessage(1);
        return -1;
    }
    Window* window = Window::GetWindow();
    //Update = &Window::Update;
    
    TestScene scene = TestScene();
    window->ChangeScene(&scene);
    //int reloadGameTick = 0;

    MSG msg = {0, 0, 0, 0, 0, NULL};
    while (window->IsRunning()) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                window->Stop();
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // reloadGameTick++;
        // if (reloadGameTick > 120) {
        //     LoadGameCode();
        //     reloadGameTick = 0;
        // }
        
        Window::Update(window, 1.0f/60.0f);
        window->Render();
        Input::EndFrame();
    }
    system("pause");
    window->Destroy();

    return (int)msg.wParam;
}