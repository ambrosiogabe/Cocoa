#include "jade/platform/JWindow.h"
#include "jade/scenes/TestScene.h"
#include "jade/util/Log.h"

#include <windows.h>
#include <windowsx.h>
#include <gl/GL.h>
#include "gl/glext.h"
#include "gl/wglext.h"

#define IMGUI_IMPL_OPENGL_LOADER_GL3W
#define IMGUI_IMPLEMENTATION
#include "imgui/misc/single_file/imgui_single_file.h"
#include "imgui/examples/imgui_impl_win32.cpp"
#include "imgui/examples/imgui_impl_opengl3.cpp"
#undef IMGUI_IMPLEMENTATION

#include "../../../build/build.h"

#define LAST(k,n) ((k) & ((1<<(n))-1))
#define MID(k,m,n) LAST((k)>>(m),((n)-(m)))

//WindowUpdateFnPt Update = nullptr;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam)) {
        return true;
    }

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
                    JWindow::KeyCallback(JADE_KEY_ESCAPE, wScanCode, action, modifierKey);
                    break;
                }
                case (VK_SPACE): {
                    JWindow::KeyCallback(JADE_KEY_SPACE, wScanCode, action, modifierKey);
                    break;
                }
                case (VK_BACK): {
                    JWindow::KeyCallback(JADE_KEY_BACKSPACE, wScanCode, action, modifierKey);
                    break;
                }
                case (VK_TAB): {
                    JWindow::KeyCallback(JADE_KEY_TAB, wScanCode, action, modifierKey);
                    break;
                }
                case (VK_CLEAR): {
                    Log::Assert(false, "Pressed unkown key 'clear'");
                    break;
                }
                case (VK_RETURN): {
                    JWindow::KeyCallback(JADE_KEY_ENTER, wScanCode, action, modifierKey);
                    break;
                }
                case (VK_LSHIFT): {
                    modifierIsPressed = action == JADE_PRESS;
                    modifierKey = JADE_MOD_SHIFT;
                    JWindow::KeyCallback(JADE_KEY_LEFT_SHIFT, wScanCode, action, modifierKey);
                    break;
                }
                case (VK_RSHIFT): {
                    modifierIsPressed = action == JADE_PRESS;
                    modifierKey = JADE_MOD_SHIFT;
                    JWindow::KeyCallback(JADE_KEY_RIGHT_SHIFT, wScanCode, action, modifierKey);
                    break;
                }
                case (VK_LCONTROL): {
                    modifierIsPressed = action == JADE_PRESS;
                    modifierKey = JADE_MOD_CONTROL;
                    JWindow::KeyCallback(JADE_KEY_LEFT_CONTROL, wScanCode, action, modifierKey);
                    break;
                }
                case (VK_RCONTROL): {
                    modifierIsPressed = action == JADE_PRESS;
                    modifierKey = JADE_MOD_CONTROL;
                    JWindow::KeyCallback(JADE_KEY_RIGHT_CONTROL, wScanCode, action, modifierKey);
                    break;
                }
                case (VK_LMENU): {
                    modifierIsPressed = action == JADE_PRESS;
                    modifierKey = JADE_MOD_ALT;
                    JWindow::KeyCallback(JADE_KEY_LEFT_ALT, wScanCode, action, modifierKey);
                    break;
                }
                case (VK_RMENU): {
                    modifierIsPressed = action == JADE_PRESS;
                    modifierKey = JADE_MOD_ALT;
                    JWindow::KeyCallback(JADE_KEY_RIGHT_ALT, wScanCode, action, modifierKey);
                    break;
                }
                case (VK_PAUSE): {
                    JWindow::KeyCallback(JADE_KEY_PAUSE, wScanCode, action, modifierKey);
                    break;
                } case (VK_CAPITAL): {
                    modifierIsPressed = action == JADE_PRESS;
                    modifierKey = JADE_MOD_CAPS_LOCK;
                    JWindow::KeyCallback(JADE_KEY_CAPS_LOCK, wScanCode, action, modifierKey);
                    break;
                } case (VK_PRIOR): {
                    JWindow::KeyCallback(JADE_KEY_PAGE_UP, wScanCode, action, modifierKey);
                    break;
                } case (VK_NEXT): {
                    JWindow::KeyCallback(JADE_KEY_PAGE_DOWN, wScanCode, action, modifierKey);
                    break;
                } case (VK_END): {
                    JWindow::KeyCallback(JADE_KEY_END, wScanCode, action, modifierKey);
                    break;
                } case (VK_HOME): {
                    JWindow::KeyCallback(JADE_KEY_HOME, wScanCode, action, modifierKey);
                    break;
                } case (VK_LEFT): {
                    JWindow::KeyCallback(JADE_KEY_LEFT, wScanCode, action, modifierKey);
                    break;
                } case (VK_UP): {
                    JWindow::KeyCallback(JADE_KEY_UP, wScanCode, action, modifierKey);
                    break;
                } case (VK_RIGHT): {
                    JWindow::KeyCallback(JADE_KEY_RIGHT, wScanCode, action, modifierKey);
                    break;
                } case (VK_DOWN): {
                    JWindow::KeyCallback(JADE_KEY_DOWN, wScanCode, action, modifierKey);
                    break;
                } case (VK_SNAPSHOT): {
                    JWindow::KeyCallback(JADE_KEY_PRINT_SCREEN, wScanCode, action, modifierKey);
                    break;
                } case (VK_INSERT): {
                    JWindow::KeyCallback(JADE_KEY_INSERT, wScanCode, action, modifierKey);
                    break;
                } case (VK_DELETE): {
                    JWindow::KeyCallback(JADE_KEY_DELETE, wScanCode, action, modifierKey);
                    break;
                } case (0x30): {
                    JWindow::KeyCallback(JADE_KEY_0, wScanCode, action, modifierKey);
                    break;
                } case (0x31): {
                    JWindow::KeyCallback(JADE_KEY_1, wScanCode, action, modifierKey);
                    break;
                } case (0x32): {
                    JWindow::KeyCallback(JADE_KEY_2, wScanCode, action, modifierKey);
                    break;
                } case (0x33): {
                    JWindow::KeyCallback(JADE_KEY_3, wScanCode, action, modifierKey);
                    break;
                } case (0x34): {
                    JWindow::KeyCallback(JADE_KEY_4, wScanCode, action, modifierKey);
                    break;
                } case (0x35): {
                    JWindow::KeyCallback(JADE_KEY_5, wScanCode, action, modifierKey);
                    break;
                } case (0x36): {
                    JWindow::KeyCallback(JADE_KEY_6, wScanCode, action, modifierKey);
                    break;
                } case (0x37): {
                    JWindow::KeyCallback(JADE_KEY_7, wScanCode, action, modifierKey);
                    break;
                } case (0x38): {
                    JWindow::KeyCallback(JADE_KEY_8, wScanCode, action, modifierKey);
                    break;
                } case (0x39): {
                    JWindow::KeyCallback(JADE_KEY_9, wScanCode, action, modifierKey);
                    break;
                } case (VK_NUMPAD0): {
                    JWindow::KeyCallback(JADE_KEY_NUMPAD_0, wScanCode, action, modifierKey);
                    break;
                } case (VK_NUMPAD1): {
                    JWindow::KeyCallback(JADE_KEY_NUMPAD_1, wScanCode, action, modifierKey);
                    break;
                } case (VK_NUMPAD2): {
                    JWindow::KeyCallback(JADE_KEY_NUMPAD_2, wScanCode, action, modifierKey);
                    break;
                } case (VK_NUMPAD3): {
                    JWindow::KeyCallback(JADE_KEY_NUMPAD_3, wScanCode, action, modifierKey);
                    break;
                } case (VK_NUMPAD4): {
                    JWindow::KeyCallback(JADE_KEY_NUMPAD_4, wScanCode, action, modifierKey);
                    break;
                } case (VK_NUMPAD5): {
                    JWindow::KeyCallback(JADE_KEY_NUMPAD_5, wScanCode, action, modifierKey);
                    break;
                } case (VK_NUMPAD6): {
                    JWindow::KeyCallback(JADE_KEY_NUMPAD_6, wScanCode, action, modifierKey);
                    break;
                } case (VK_NUMPAD7): {
                    JWindow::KeyCallback(JADE_KEY_NUMPAD_7, wScanCode, action, modifierKey);
                    break;
                } case (VK_NUMPAD8): {
                    JWindow::KeyCallback(JADE_KEY_NUMPAD_8, wScanCode, action, modifierKey);
                    break;
                } case (VK_NUMPAD9): {
                    JWindow::KeyCallback(JADE_KEY_NUMPAD_9, wScanCode, action, modifierKey);
                    break;
                } case (VK_F1): {
                    JWindow::KeyCallback(JADE_KEY_F1, wScanCode, action, modifierKey);
                    break;
                } case (VK_F2): {
                    JWindow::KeyCallback(JADE_KEY_F2, wScanCode, action, modifierKey);
                    break;
                } case (VK_F3): {
                    JWindow::KeyCallback(JADE_KEY_F3, wScanCode, action, modifierKey);
                    break;
                } case (VK_F4): {
                    JWindow::KeyCallback(JADE_KEY_F4, wScanCode, action, modifierKey);
                    break;
                } case (VK_F5): {
                    JWindow::KeyCallback(JADE_KEY_F5, wScanCode, action, modifierKey);
                    break;
                } case (VK_F6): {
                    JWindow::KeyCallback(JADE_KEY_F6, wScanCode, action, modifierKey);
                    break;
                } case (VK_F7): {
                    JWindow::KeyCallback(JADE_KEY_F7, wScanCode, action, modifierKey);
                    break;
                } case (VK_F8): {
                    JWindow::KeyCallback(JADE_KEY_F8, wScanCode, action, modifierKey);
                    break;
                } case (VK_F9): {
                    JWindow::KeyCallback(JADE_KEY_F9, wScanCode, action, modifierKey);
                    break;
                } case (VK_F10): {
                    JWindow::KeyCallback(JADE_KEY_F10, wScanCode, action, modifierKey);
                    break;
                } case (VK_F11): {
                    JWindow::KeyCallback(JADE_KEY_F11, wScanCode, action, modifierKey);
                    break;
                } case (VK_F12): {
                    JWindow::KeyCallback(JADE_KEY_F12, wScanCode, action, modifierKey);
                    break;
                } case (VK_F13): {
                    JWindow::KeyCallback(JADE_KEY_F13, wScanCode, action, modifierKey);
                    break;
                } case (VK_F14): {
                    JWindow::KeyCallback(JADE_KEY_F14, wScanCode, action, modifierKey);
                    break;
                } case (VK_F15): {
                    JWindow::KeyCallback(JADE_KEY_F15, wScanCode, action, modifierKey);
                    break;
                } case (VK_F16): {
                    JWindow::KeyCallback(JADE_KEY_F16, wScanCode, action, modifierKey);
                    break;
                } case (VK_F17): {
                    JWindow::KeyCallback(JADE_KEY_F17, wScanCode, action, modifierKey);
                    break;
                } case (VK_F18): {
                    JWindow::KeyCallback(JADE_KEY_F18, wScanCode, action, modifierKey);
                    break;
                } case (VK_F19): {
                    JWindow::KeyCallback(JADE_KEY_F19, wScanCode, action, modifierKey);
                    break;
                } case (VK_F20): {
                    JWindow::KeyCallback(JADE_KEY_F20, wScanCode, action, modifierKey);
                    break;
                } case (VK_F21): {
                    JWindow::KeyCallback(JADE_KEY_F21, wScanCode, action, modifierKey);
                    break;
                } case (VK_F22): {
                    JWindow::KeyCallback(JADE_KEY_F22, wScanCode, action, modifierKey);
                    break;
                } case (VK_F23): {
                    JWindow::KeyCallback(JADE_KEY_F23, wScanCode, action, modifierKey);
                    break;
                } case (VK_F24): {
                    JWindow::KeyCallback(JADE_KEY_F24, wScanCode, action, modifierKey);
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
                    JWindow::KeyCallback((int)wParam, wScanCode, action, modifierKey);
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
                JWindow::MouseButtonCallback(button, JADE_PRESS, modifierKey);
            }
            break;
        }
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDBLCLK: {
            int button = wParam == MK_LBUTTON ? JADE_MOUSE_BUTTON_LEFT : wParam == MK_RBUTTON ? JADE_MOUSE_BUTTON_RIGHT : wParam == MK_MBUTTON ? JADE_MOUSE_BUTTON_MIDDLE : -1;
            if (button != -1) {
                JWindow::MouseButtonCallback(button, JADE_DOUBLE_CLICK, modifierKey);
            }
            break;
        }
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP: {
            int button = message == WM_LBUTTONUP ? JADE_MOUSE_BUTTON_LEFT : message == WM_RBUTTONUP ? JADE_MOUSE_BUTTON_RIGHT : message == WM_MBUTTONUP ? JADE_MOUSE_BUTTON_MIDDLE : -1;
            if (button != -1) {
                JWindow::MouseButtonCallback(button, JADE_RELEASE, modifierKey);
            }
            break;
        }

        // Mouse move events
        case WM_MOUSEMOVE: {
            int xpos = GET_X_LPARAM(lParam);
            int ypos = GET_Y_LPARAM(lParam);
            JWindow::CursorCallback((double)xpos, (double)ypos);
            break;
        }

        // Mouse scroll events
        case WM_MOUSEWHEEL: {
            int wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
            // TODO: Find out if horizontal scroll is supported by windows
            JWindow::ScrollCallback(0.0, (double)wheelDelta);
            break;
        }

        // JWindow resize events
        case WM_SIZE: {
            RECT rect;
            GetWindowRect(hWnd, &rect);
            int newHeight = rect.bottom - rect.top;
            int newWidth = rect.right - rect.left;
            JWindow::ResizeCallback(newWidth, newHeight);
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

    if (Win32Window::Create(hInstance, nCmdShow) != 0) {
        PostQuitMessage(1);
        return -1;
    }
    JWindow* JWindow = JWindow::Get();
    //Update = &JWindow::Update;
    
    TestScene scene = TestScene();
    JWindow->ChangeScene(&scene);
    //int reloadGameTick = 0;

    MSG msg = {0, 0, 0, 0, 0, NULL};
    while (JWindow->IsRunning()) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                JWindow->Stop();
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // reloadGameTick++;
        // if (reloadGameTick > 120) {
        //     LoadGameCode();
        //     reloadGameTick = 0;
        // }
        
        JWindow::Update(JWindow, 1.0f/60.0f);
        JWindow->Render();
        Input::EndFrame();
    }
    system("pause");
    JWindow->Destroy();

    return (int)msg.wParam;
}