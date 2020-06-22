#pragma once

#include "scenes/TestScene.h"
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
                    Jade::Window::KeyCallback(JADE_KEY_ESCAPE, wScanCode, action, modifierKey);
                    break;
                }
                case (VK_SPACE): {
                    Jade::Window::KeyCallback(JADE_KEY_SPACE, wScanCode, action, modifierKey);
                    break;
                }
                case (VK_BACK): {
                    Jade::Window::KeyCallback(JADE_KEY_BACKSPACE, wScanCode, action, modifierKey);
                    break;
                }
                case (VK_TAB): {
                    Jade::Window::KeyCallback(JADE_KEY_TAB, wScanCode, action, modifierKey);
                    break;
                }
                case (VK_CLEAR): {
                    Jade::Log::Assert(false, "Pressed unkown key 'clear'");
                    break;
                }
                case (VK_RETURN): {
                    Jade::Window::KeyCallback(JADE_KEY_ENTER, wScanCode, action, modifierKey);
                    break;
                }
                case (VK_LSHIFT): {
                    modifierIsPressed = action == JADE_PRESS;
                    modifierKey = JADE_MOD_SHIFT;
                    Jade::Window::KeyCallback(JADE_KEY_LEFT_SHIFT, wScanCode, action, modifierKey);
                    break;
                }
                case (VK_RSHIFT): {
                    modifierIsPressed = action == JADE_PRESS;
                    modifierKey = JADE_MOD_SHIFT;
                    Jade::Window::KeyCallback(JADE_KEY_RIGHT_SHIFT, wScanCode, action, modifierKey);
                    break;
                }
                case (VK_LCONTROL): {
                    modifierIsPressed = action == JADE_PRESS;
                    modifierKey = JADE_MOD_CONTROL;
                    Jade::Window::KeyCallback(JADE_KEY_LEFT_CONTROL, wScanCode, action, modifierKey);
                    break;
                }
                case (VK_RCONTROL): {
                    modifierIsPressed = action == JADE_PRESS;
                    modifierKey = JADE_MOD_CONTROL;
                    Jade::Window::KeyCallback(JADE_KEY_RIGHT_CONTROL, wScanCode, action, modifierKey);
                    break;
                }
                case (VK_LMENU): {
                    modifierIsPressed = action == JADE_PRESS;
                    modifierKey = JADE_MOD_ALT;
                    Jade::Window::KeyCallback(JADE_KEY_LEFT_ALT, wScanCode, action, modifierKey);
                    break;
                }
                case (VK_RMENU): {
                    modifierIsPressed = action == JADE_PRESS;
                    modifierKey = JADE_MOD_ALT;
                    Jade::Window::KeyCallback(JADE_KEY_RIGHT_ALT, wScanCode, action, modifierKey);
                    break;
                }
                case (VK_PAUSE): {
                    Jade::Window::KeyCallback(JADE_KEY_PAUSE, wScanCode, action, modifierKey);
                    break;
                } case (VK_CAPITAL): {
                    modifierIsPressed = action == JADE_PRESS;
                    modifierKey = JADE_MOD_CAPS_LOCK;
                    Jade::Window::KeyCallback(JADE_KEY_CAPS_LOCK, wScanCode, action, modifierKey);
                    break;
                } case (VK_PRIOR): {
                    Jade::Window::KeyCallback(JADE_KEY_PAGE_UP, wScanCode, action, modifierKey);
                    break;
                } case (VK_NEXT): {
                    Jade::Window::KeyCallback(JADE_KEY_PAGE_DOWN, wScanCode, action, modifierKey);
                    break;
                } case (VK_END): {
                    Jade::Window::KeyCallback(JADE_KEY_END, wScanCode, action, modifierKey);
                    break;
                } case (VK_HOME): {
                    Jade::Window::KeyCallback(JADE_KEY_HOME, wScanCode, action, modifierKey);
                    break;
                } case (VK_LEFT): {
                    Jade::Window::KeyCallback(JADE_KEY_LEFT, wScanCode, action, modifierKey);
                    break;
                } case (VK_UP): {
                    Jade::Window::KeyCallback(JADE_KEY_UP, wScanCode, action, modifierKey);
                    break;
                } case (VK_RIGHT): {
                    Jade::Window::KeyCallback(JADE_KEY_RIGHT, wScanCode, action, modifierKey);
                    break;
                } case (VK_DOWN): {
                    Jade::Window::KeyCallback(JADE_KEY_DOWN, wScanCode, action, modifierKey);
                    break;
                } case (VK_SNAPSHOT): {
                    Jade::Window::KeyCallback(JADE_KEY_PRINT_SCREEN, wScanCode, action, modifierKey);
                    break;
                } case (VK_INSERT): {
                    Jade::Window::KeyCallback(JADE_KEY_INSERT, wScanCode, action, modifierKey);
                    break;
                } case (VK_DELETE): {
                    Jade::Window::KeyCallback(JADE_KEY_DELETE, wScanCode, action, modifierKey);
                    break;
                } case (0x30): {
                    Jade::Window::KeyCallback(JADE_KEY_0, wScanCode, action, modifierKey);
                    break;
                } case (0x31): {
                    Jade::Window::KeyCallback(JADE_KEY_1, wScanCode, action, modifierKey);
                    break;
                } case (0x32): {
                    Jade::Window::KeyCallback(JADE_KEY_2, wScanCode, action, modifierKey);
                    break;
                } case (0x33): {
                    Jade::Window::KeyCallback(JADE_KEY_3, wScanCode, action, modifierKey);
                    break;
                } case (0x34): {
                    Jade::Window::KeyCallback(JADE_KEY_4, wScanCode, action, modifierKey);
                    break;
                } case (0x35): {
                    Jade::Window::KeyCallback(JADE_KEY_5, wScanCode, action, modifierKey);
                    break;
                } case (0x36): {
                    Jade::Window::KeyCallback(JADE_KEY_6, wScanCode, action, modifierKey);
                    break;
                } case (0x37): {
                    Jade::Window::KeyCallback(JADE_KEY_7, wScanCode, action, modifierKey);
                    break;
                } case (0x38): {
                    Jade::Window::KeyCallback(JADE_KEY_8, wScanCode, action, modifierKey);
                    break;
                } case (0x39): {
                    Jade::Window::KeyCallback(JADE_KEY_9, wScanCode, action, modifierKey);
                    break;
                } case (VK_NUMPAD0): {
                    Jade::Window::KeyCallback(JADE_KEY_NUMPAD_0, wScanCode, action, modifierKey);
                    break;
                } case (VK_NUMPAD1): {
                    Jade::Window::KeyCallback(JADE_KEY_NUMPAD_1, wScanCode, action, modifierKey);
                    break;
                } case (VK_NUMPAD2): {
                    Jade::Window::KeyCallback(JADE_KEY_NUMPAD_2, wScanCode, action, modifierKey);
                    break;
                } case (VK_NUMPAD3): {
                    Jade::Window::KeyCallback(JADE_KEY_NUMPAD_3, wScanCode, action, modifierKey);
                    break;
                } case (VK_NUMPAD4): {
                    Jade::Window::KeyCallback(JADE_KEY_NUMPAD_4, wScanCode, action, modifierKey);
                    break;
                } case (VK_NUMPAD5): {
                    Jade::Window::KeyCallback(JADE_KEY_NUMPAD_5, wScanCode, action, modifierKey);
                    break;
                } case (VK_NUMPAD6): {
                    Jade::Window::KeyCallback(JADE_KEY_NUMPAD_6, wScanCode, action, modifierKey);
                    break;
                } case (VK_NUMPAD7): {
                    Jade::Window::KeyCallback(JADE_KEY_NUMPAD_7, wScanCode, action, modifierKey);
                    break;
                } case (VK_NUMPAD8): {
                    Jade::Window::KeyCallback(JADE_KEY_NUMPAD_8, wScanCode, action, modifierKey);
                    break;
                } case (VK_NUMPAD9): {
                    Jade::Window::KeyCallback(JADE_KEY_NUMPAD_9, wScanCode, action, modifierKey);
                    break;
                } case (VK_F1): {
                    Jade::Window::KeyCallback(JADE_KEY_F1, wScanCode, action, modifierKey);
                    break;
                } case (VK_F2): {
                    Jade::Window::KeyCallback(JADE_KEY_F2, wScanCode, action, modifierKey);
                    break;
                } case (VK_F3): {
                    Jade::Window::KeyCallback(JADE_KEY_F3, wScanCode, action, modifierKey);
                    break;
                } case (VK_F4): {
                    Jade::Window::KeyCallback(JADE_KEY_F4, wScanCode, action, modifierKey);
                    break;
                } case (VK_F5): {
                    Jade::Window::KeyCallback(JADE_KEY_F5, wScanCode, action, modifierKey);
                    break;
                } case (VK_F6): {
                    Jade::Window::KeyCallback(JADE_KEY_F6, wScanCode, action, modifierKey);
                    break;
                } case (VK_F7): {
                    Jade::Window::KeyCallback(JADE_KEY_F7, wScanCode, action, modifierKey);
                    break;
                } case (VK_F8): {
                    Jade::Window::KeyCallback(JADE_KEY_F8, wScanCode, action, modifierKey);
                    break;
                } case (VK_F9): {
                    Jade::Window::KeyCallback(JADE_KEY_F9, wScanCode, action, modifierKey);
                    break;
                } case (VK_F10): {
                    Jade::Window::KeyCallback(JADE_KEY_F10, wScanCode, action, modifierKey);
                    break;
                } case (VK_F11): {
                    Jade::Window::KeyCallback(JADE_KEY_F11, wScanCode, action, modifierKey);
                    break;
                } case (VK_F12): {
                    Jade::Window::KeyCallback(JADE_KEY_F12, wScanCode, action, modifierKey);
                    break;
                } case (VK_F13): {
                    Jade::Window::KeyCallback(JADE_KEY_F13, wScanCode, action, modifierKey);
                    break;
                } case (VK_F14): {
                    Jade::Window::KeyCallback(JADE_KEY_F14, wScanCode, action, modifierKey);
                    break;
                } case (VK_F15): {
                    Jade::Window::KeyCallback(JADE_KEY_F15, wScanCode, action, modifierKey);
                    break;
                } case (VK_F16): {
                    Jade::Window::KeyCallback(JADE_KEY_F16, wScanCode, action, modifierKey);
                    break;
                } case (VK_F17): {
                    Jade::Window::KeyCallback(JADE_KEY_F17, wScanCode, action, modifierKey);
                    break;
                } case (VK_F18): {
                    Jade::Window::KeyCallback(JADE_KEY_F18, wScanCode, action, modifierKey);
                    break;
                } case (VK_F19): {
                    Jade::Window::KeyCallback(JADE_KEY_F19, wScanCode, action, modifierKey);
                    break;
                } case (VK_F20): {
                    Jade::Window::KeyCallback(JADE_KEY_F20, wScanCode, action, modifierKey);
                    break;
                } case (VK_F21): {
                    Jade::Window::KeyCallback(JADE_KEY_F21, wScanCode, action, modifierKey);
                    break;
                } case (VK_F22): {
                    Jade::Window::KeyCallback(JADE_KEY_F22, wScanCode, action, modifierKey);
                    break;
                } case (VK_F23): {
                    Jade::Window::KeyCallback(JADE_KEY_F23, wScanCode, action, modifierKey);
                    break;
                } case (VK_F24): {
                    Jade::Window::KeyCallback(JADE_KEY_F24, wScanCode, action, modifierKey);
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
                    Jade::Window::KeyCallback((int)wParam, wScanCode, action, modifierKey);
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
                Jade::Window::MouseButtonCallback(button, JADE_PRESS, modifierKey);
            }
            break;
        }
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDBLCLK: {
            int button = wParam == MK_LBUTTON ? JADE_MOUSE_BUTTON_LEFT : wParam == MK_RBUTTON ? JADE_MOUSE_BUTTON_RIGHT : wParam == MK_MBUTTON ? JADE_MOUSE_BUTTON_MIDDLE : -1;
            if (button != -1) {
                Jade::Window::MouseButtonCallback(button, JADE_DOUBLE_CLICK, modifierKey);
            }
            break;
        }
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP: {
            int button = message == WM_LBUTTONUP ? JADE_MOUSE_BUTTON_LEFT : message == WM_RBUTTONUP ? JADE_MOUSE_BUTTON_RIGHT : message == WM_MBUTTONUP ? JADE_MOUSE_BUTTON_MIDDLE : -1;
            if (button != -1) {
                Jade::Window::MouseButtonCallback(button, JADE_RELEASE, modifierKey);
            }
            break;
        }

        // Mouse move events
        case WM_MOUSEMOVE: {
            int xpos = GET_X_LPARAM(lParam);
            int ypos = GET_Y_LPARAM(lParam);
            Jade::Window::CursorCallback((double)xpos, (double)ypos);
            break;
        }

        // Mouse scroll events
        case WM_MOUSEWHEEL: {
            int wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
            // TODO: Find out if horizontal scroll is supported by windows
            Jade::Window::ScrollCallback(0.0, (double)wheelDelta);
            break;
        }

        // Window resize events
        case WM_SIZE: {
            RECT rect;
            GetWindowRect(hWnd, &rect);
            int newHeight = rect.bottom - rect.top;
            int newWidth = rect.right - rect.left;
            Jade::Window::ResizeCallback(newWidth, newHeight);
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

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    Jade::Win32Window::InitWin32(hInstance);    

    Jade::Window::InitHint(JADE_WH_ALLOC_CONSOLE, true);
    Jade::Window::InitHint(JADE_WH_CENTER_CURSOR, true);
    Jade::Window* win = Jade::Window::CreateWindow(1920, 1080, "Some Title");

    Jade::TestScene scene = Jade::TestScene();
    win->ChangeScene(&scene);

    while (win->IsRunning()) {
        win->PollEvents();
        Jade::Window::Update(win, 1.0f/60.0f);
        win->Render();
        Jade::Input::EndFrame();
    }

    system("pause");
    win->Destroy();
    return 0;
}