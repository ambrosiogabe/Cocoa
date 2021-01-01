// TODO: This code is now deprecated in favor of GLFW, may come back to it at some point
#if 0

#include "platform/Window.h"
#include "jade/util/Log.h"
#include "jade/events/Input.h"

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <io.h>

#define GL_LITE_IMPLEMENTATION
#include <gl/gl3w.h>
#undef GL_LITE_IMPLEMENTATION

namespace Jade {
    bool Win32Window::m_Initialized = false;
    HINSTANCE Win32Window::m_HINSTANCE = {};
    HGLRC Win32Window::m_GlobalRC = NULL;

    PFNWGLSWAPINTERVALEXTPROC Win32Window::wglSwapIntervalEXT = nullptr;
    PFNWGLCHOOSEPIXELFORMATARBPROC Win32Window::wglChoosePixelFormatARB = nullptr;
    PFNWGLCREATECONTEXTATTRIBSARBPROC Win32Window::wglCreateContextAttribsARB = nullptr;

    void Win32Window::DefaultResizeCallback(HWND wnd, int width, int height) {
        for (Window* window : s_Windows) {
            Win32Window* win = static_cast<Win32Window*>(window);
            if (win->WND == wnd) {
                win->m_Width = width;
                win->m_Height = height;
                if (window->m_ResizeCallback != nullptr) {
                    window->m_ResizeCallback(window, width, height);
                }
                break;
            }
        }
    }

    void Win32Window::DefaultMouseButtonCallback(HWND wnd, int button, int action, int mods) {
        for (Window* window : s_Windows) {
            Win32Window* win = static_cast<Win32Window*>(window);
            if (win->WND == wnd) {
                if (window->m_MouseButtonCallback != nullptr) {
                    window->m_MouseButtonCallback(window, button, action, mods);
                }
                break;
            }
        }
    }

    void Win32Window::DefaultCursorCallback(HWND wnd, double xpos, double ypos) {
        for (Window* window : s_Windows) {
            Win32Window* win = static_cast<Win32Window*>(window);
            if (win->WND == wnd) {
                if (window->m_CursorCallback != nullptr) {
                    window->m_CursorCallback(window, xpos, ypos);
                }
                break;
            }
        }
    }

    void Win32Window::DefaultScrollCalback(HWND wnd, double xoffset, double yoffset) {
        for (Window* window : s_Windows) {
            Win32Window* win = static_cast<Win32Window*>(window);
            if (win->WND == wnd) {
                if (window->m_ScrollCallback != nullptr) {
                    window->m_ScrollCallback(window, xoffset, yoffset);
                }
                break;
            }
        }
    }

    void Win32Window::DefaultKeyCallback(HWND wnd, int key, int scancode, int action, int mods) {
        for (Window* window : s_Windows) {
            Win32Window* win = static_cast<Win32Window*>(window);
            if (win->WND == wnd) {
                if (window->m_KeyCallback != nullptr) {
                    window->m_KeyCallback(window, key, scancode, action, mods);
                }
                break;
            }
        }
    }

    void Win32Window::DefaultCloseCallback(HWND wnd) {
        for (Window* window : s_Windows) {
            Win32Window* win = static_cast<Win32Window*>(window);
            if (win->WND == wnd) {
                window->m_WindowShouldClose = true;
                if (window->m_CloseCallback != nullptr) {
                    window->m_CloseCallback(window);
                }
                break;
            }
        }
    }

    Window* Win32Window::GetWindow(HWND window) {
        for (Window* win : s_Windows) {
            Win32Window* win32Win = static_cast<Win32Window*>(win);
            if (win32Win->WND == window) {
                return win;
            }
        }

        return nullptr;
    }

    void Win32Window::ShowMessage(LPCSTR message) {
        MessageBoxA(0, message, "Win32Window::Create", MB_ICONERROR);
    }

    static HICON CreateIconFromBytes(HDC DC, int width, int height, uint32* bytes) {
        HICON hIcon = NULL;

        ICONINFO iconInfo = {
            TRUE, // fIcon, set to true if this is an icon, set to false if this is a cursor
            NULL, // xHotspot, set to null for icons
            NULL, // yHotspot, set to null for icons
            NULL, // Monochrome bitmap mask, set to null initially
            NULL  // Color bitmap mask, set to null initially
        };

        uint32* rawBitmap = new uint32[width * height];

        ULONG uWidth = (ULONG)width;
        ULONG uHeight = (ULONG)height;
        uint32* bitmapPtr = rawBitmap;
        for (ULONG y = 0; y < uHeight; y++) {
            for (ULONG x = 0; x < uWidth; x++) {
                // Bytes are expected to be in RGB order (8 bits each)
                // Swap G and B bytes, so that it is in BGR order for windows
                uint32 byte = bytes[x + y * width];
                uint8 A = (byte & 0xff000000) >> 24;
                uint8 R = (byte & 0xff0000) >> 16;
                uint8 G = (byte & 0xff00) >> 8;
                uint8 B = (byte & 0xff);
                *bitmapPtr = (A << 24) | (R << 16) | (G << 8) | B;
                bitmapPtr++;
            }
        }

        iconInfo.hbmColor = CreateBitmap(width, height, 1, 32, rawBitmap);
        if (iconInfo.hbmColor) {
            iconInfo.hbmMask = CreateCompatibleBitmap(DC, width, height);
            if (iconInfo.hbmMask) {
                hIcon = CreateIconIndirect(&iconInfo);
                if (hIcon == NULL) {
                    Log::Warning("Failed to create icon.");
                }
                DeleteObject(iconInfo.hbmMask);
            } else {
                Log::Warning("Failed to create color mask.");
            }
            DeleteObject(iconInfo.hbmColor);
        } else {
            Log::Warning("Failed to create bitmap mask.");
        }

        delete[] rawBitmap;

        return hIcon;
    }

    void Win32Window::_SetWindowIcon(int count, const WindowImage* images) {
        for (int i=0; i < count; i++) {
            WindowImage image = images[i];
            HICON icon = CreateIconFromBytes(DC, image.m_Width, image.m_Height, image.m_Pixels);
            m_Icons.push_back(icon);
            SendMessage(WND, WM_SETICON, ICON_SMALL, (LPARAM)icon);
            SendMessage(WND, WM_SETICON, ICON_BIG, (LPARAM)icon);
            SendMessage(WND, WM_SETICON, ICON_SMALL2, (LPARAM)icon);
        }
    }

    static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        #define LAST(k,n) ((k) & ((1<<(n))-1))
        #define MID(k,m,n) LAST((k)>>(m),((n)-(m)))
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
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_ESCAPE, wScanCode, action, modifierKey);
                        break;
                    }
                    case (VK_SPACE): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_SPACE, wScanCode, action, modifierKey);
                        break;
                    }
                    case (VK_BACK): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_BACKSPACE, wScanCode, action, modifierKey);
                        break;
                    }
                    case (VK_TAB): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_TAB, wScanCode, action, modifierKey);
                        break;
                    }
                    case (VK_CLEAR): {
                        Jade::Log::Assert(false, "Pressed unkown key 'clear'");
                        break;
                    }
                    case (VK_RETURN): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_ENTER, wScanCode, action, modifierKey);
                        break;
                    }
                    case (VK_LSHIFT): {
                        modifierIsPressed = action == JADE_PRESS;
                        modifierKey = JADE_MOD_SHIFT;
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_LEFT_SHIFT, wScanCode, action, modifierKey);
                        break;
                    }
                    case (VK_RSHIFT): {
                        modifierIsPressed = action == JADE_PRESS;
                        modifierKey = JADE_MOD_SHIFT;
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_RIGHT_SHIFT, wScanCode, action, modifierKey);
                        break;
                    }
                    case (VK_LCONTROL): {
                        modifierIsPressed = action == JADE_PRESS;
                        modifierKey = JADE_MOD_CONTROL;
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_LEFT_CONTROL, wScanCode, action, modifierKey);
                        break;
                    }
                    case (VK_RCONTROL): {
                        modifierIsPressed = action == JADE_PRESS;
                        modifierKey = JADE_MOD_CONTROL;
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_RIGHT_CONTROL, wScanCode, action, modifierKey);
                        break;
                    }
                    case (VK_LMENU): {
                        modifierIsPressed = action == JADE_PRESS;
                        modifierKey = JADE_MOD_ALT;
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_LEFT_ALT, wScanCode, action, modifierKey);
                        break;
                    }
                    case (VK_RMENU): {
                        modifierIsPressed = action == JADE_PRESS;
                        modifierKey = JADE_MOD_ALT;
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_RIGHT_ALT, wScanCode, action, modifierKey);
                        break;
                    }
                    case (VK_PAUSE): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_PAUSE, wScanCode, action, modifierKey);
                        break;
                    } case (VK_CAPITAL): {
                        modifierIsPressed = action == JADE_PRESS;
                        modifierKey = JADE_MOD_CAPS_LOCK;
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_CAPS_LOCK, wScanCode, action, modifierKey);
                        break;
                    } case (VK_PRIOR): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_PAGE_UP, wScanCode, action, modifierKey);
                        break;
                    } case (VK_NEXT): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_PAGE_DOWN, wScanCode, action, modifierKey);
                        break;
                    } case (VK_END): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_END, wScanCode, action, modifierKey);
                        break;
                    } case (VK_HOME): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_HOME, wScanCode, action, modifierKey);
                        break;
                    } case (VK_LEFT): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_LEFT, wScanCode, action, modifierKey);
                        break;
                    } case (VK_UP): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_UP, wScanCode, action, modifierKey);
                        break;
                    } case (VK_RIGHT): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_RIGHT, wScanCode, action, modifierKey);
                        break;
                    } case (VK_DOWN): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_DOWN, wScanCode, action, modifierKey);
                        break;
                    } case (VK_SNAPSHOT): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_PRINT_SCREEN, wScanCode, action, modifierKey);
                        break;
                    } case (VK_INSERT): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_INSERT, wScanCode, action, modifierKey);
                        break;
                    } case (VK_DELETE): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_DELETE, wScanCode, action, modifierKey);
                        break;
                    } case (0x30): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_0, wScanCode, action, modifierKey);
                        break;
                    } case (0x31): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_1, wScanCode, action, modifierKey);
                        break;
                    } case (0x32): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_2, wScanCode, action, modifierKey);
                        break;
                    } case (0x33): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_3, wScanCode, action, modifierKey);
                        break;
                    } case (0x34): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_4, wScanCode, action, modifierKey);
                        break;
                    } case (0x35): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_5, wScanCode, action, modifierKey);
                        break;
                    } case (0x36): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_6, wScanCode, action, modifierKey);
                        break;
                    } case (0x37): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_7, wScanCode, action, modifierKey);
                        break;
                    } case (0x38): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_8, wScanCode, action, modifierKey);
                        break;
                    } case (0x39): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_9, wScanCode, action, modifierKey);
                        break;
                    } case (VK_NUMPAD0): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_NUMPAD_0, wScanCode, action, modifierKey);
                        break;
                    } case (VK_NUMPAD1): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_NUMPAD_1, wScanCode, action, modifierKey);
                        break;
                    } case (VK_NUMPAD2): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_NUMPAD_2, wScanCode, action, modifierKey);
                        break;
                    } case (VK_NUMPAD3): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_NUMPAD_3, wScanCode, action, modifierKey);
                        break;
                    } case (VK_NUMPAD4): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_NUMPAD_4, wScanCode, action, modifierKey);
                        break;
                    } case (VK_NUMPAD5): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_NUMPAD_5, wScanCode, action, modifierKey);
                        break;
                    } case (VK_NUMPAD6): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_NUMPAD_6, wScanCode, action, modifierKey);
                        break;
                    } case (VK_NUMPAD7): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_NUMPAD_7, wScanCode, action, modifierKey);
                        break;
                    } case (VK_NUMPAD8): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_NUMPAD_8, wScanCode, action, modifierKey);
                        break;
                    } case (VK_NUMPAD9): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_NUMPAD_9, wScanCode, action, modifierKey);
                        break;
                    } case (VK_F1): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_F1, wScanCode, action, modifierKey);
                        break;
                    } case (VK_F2): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_F2, wScanCode, action, modifierKey);
                        break;
                    } case (VK_F3): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_F3, wScanCode, action, modifierKey);
                        break;
                    } case (VK_F4): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_F4, wScanCode, action, modifierKey);
                        break;
                    } case (VK_F5): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_F5, wScanCode, action, modifierKey);
                        break;
                    } case (VK_F6): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_F6, wScanCode, action, modifierKey);
                        break;
                    } case (VK_F7): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_F7, wScanCode, action, modifierKey);
                        break;
                    } case (VK_F8): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_F8, wScanCode, action, modifierKey);
                        break;
                    } case (VK_F9): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_F9, wScanCode, action, modifierKey);
                        break;
                    } case (VK_F10): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_F10, wScanCode, action, modifierKey);
                        break;
                    } case (VK_F11): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_F11, wScanCode, action, modifierKey);
                        break;
                    } case (VK_F12): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_F12, wScanCode, action, modifierKey);
                        break;
                    } case (VK_F13): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_F13, wScanCode, action, modifierKey);
                        break;
                    } case (VK_F14): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_F14, wScanCode, action, modifierKey);
                        break;
                    } case (VK_F15): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_F15, wScanCode, action, modifierKey);
                        break;
                    } case (VK_F16): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_F16, wScanCode, action, modifierKey);
                        break;
                    } case (VK_F17): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_F17, wScanCode, action, modifierKey);
                        break;
                    } case (VK_F18): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_F18, wScanCode, action, modifierKey);
                        break;
                    } case (VK_F19): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_F19, wScanCode, action, modifierKey);
                        break;
                    } case (VK_F20): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_F20, wScanCode, action, modifierKey);
                        break;
                    } case (VK_F21): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_F21, wScanCode, action, modifierKey);
                        break;
                    } case (VK_F22): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_F22, wScanCode, action, modifierKey);
                        break;
                    } case (VK_F23): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_F23, wScanCode, action, modifierKey);
                        break;
                    } case (VK_F24): {
                        Jade::Win32Window::DefaultKeyCallback(hWnd, JADE_KEY_F24, wScanCode, action, modifierKey);
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
                        Jade::Win32Window::DefaultKeyCallback(hWnd, (int)wParam, wScanCode, action, modifierKey);
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
                    Jade::Win32Window::DefaultMouseButtonCallback(hWnd, button, JADE_PRESS, modifierKey);
                }
                break;
            }
            case WM_LBUTTONDBLCLK:
            case WM_RBUTTONDBLCLK:
            case WM_MBUTTONDBLCLK: {
                int button = wParam == MK_LBUTTON ? JADE_MOUSE_BUTTON_LEFT : wParam == MK_RBUTTON ? JADE_MOUSE_BUTTON_RIGHT : wParam == MK_MBUTTON ? JADE_MOUSE_BUTTON_MIDDLE : -1;
                if (button != -1) {
                    Jade::Win32Window::DefaultMouseButtonCallback(hWnd, button, JADE_DOUBLE_CLICK, modifierKey);
                }
                break;
            }
            case WM_LBUTTONUP:
            case WM_RBUTTONUP:
            case WM_MBUTTONUP: {
                int button = message == WM_LBUTTONUP ? JADE_MOUSE_BUTTON_LEFT : message == WM_RBUTTONUP ? JADE_MOUSE_BUTTON_RIGHT : message == WM_MBUTTONUP ? JADE_MOUSE_BUTTON_MIDDLE : -1;
                if (button != -1) {
                    Jade::Win32Window::DefaultMouseButtonCallback(hWnd, button, JADE_RELEASE, modifierKey);
                }
                break;
            }

            // Mouse move events
            case WM_MOUSEMOVE: {
                int xpos = GET_X_LPARAM(lParam);
                int ypos = GET_Y_LPARAM(lParam);
                Jade::Win32Window::DefaultCursorCallback(hWnd, (double)xpos, (double)ypos);
                break;
            }

            // Mouse scroll events
            case WM_MOUSEWHEEL: {
                int wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
                // TODO: Find out if horizontal scroll is supported by windows
                Jade::Win32Window::DefaultScrollCalback(hWnd, 0.0, (double)wheelDelta);
                break;
            }

            // Window resize events
            case WM_SIZE: {
                RECT rect;
                GetWindowRect(hWnd, &rect);
                int newHeight = rect.bottom - rect.top;
                int newWidth = rect.right - rect.left;
                Jade::Win32Window::DefaultResizeCallback(hWnd, newWidth, newHeight);
                break;
            }

            // Window move events
            case WM_MOVE: {
                int xpos = (int)(short)LOWORD(lParam);
                int ypos = (int)(short)HIWORD(lParam);
                Window* win = Jade::Win32Window::GetWindow(hWnd);
                Jade::Window::SetWindowPos(win, xpos, ypos);
                break;
            }
            
            case WM_QUIT:
            case WM_CLOSE: {
                Jade::Win32Window::DefaultCloseCallback(hWnd);
                PostQuitMessage(1);
                break;
            }
            default: {
                result = DefWindowProcA(hWnd, message, wParam, lParam);
            }
        }

        return result;
    }

    static ATOM registerClass(HINSTANCE hInstance) {
        WNDCLASSEXA wcex;
        ZeroMemory(&wcex, sizeof(wcex));
        wcex.cbSize = sizeof(wcex);
        wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wcex.lpfnWndProc = WindowProcedure;
        wcex.hInstance = hInstance;
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex.hIcon = NULL;
        wcex.hIconSm = NULL;
        wcex.lpszClassName = "Core";

        return RegisterClassExA(&wcex);
    }

    void Win32Window::_SwapBuffers() {
        ::SwapBuffers(DC);
    }

    void Win32Window::_Destroy() {
        wglMakeCurrent(NULL, NULL);
        if (RC) {
            wglDeleteContext(RC);
        }
        if (DC) {
            ReleaseDC(WND, DC);
        }
        if (WND) {
            DestroyWindow(WND);
        }
    }

    void Win32Window::_Hide() {
        if (WND) {
            ShowWindow(WND, SW_HIDE);
        }
    }

    void Win32Window::_Show() {
        ShowWindow(WND, SW_SHOWNORMAL);
        if (s_InitFocusOnShow) {
            SetFocus(WND);
        }
    }

    void Win32Window::_PollEvents() {
        MSG msg = {0, 0, 0, 0, 0, NULL};
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    void Win32Window::_SetWindowPos(int x, int y) {
        m_XPos = x;
        m_YPos = y;

        HWND zIndex = HWND_NOTOPMOST;
        if (s_InitFloating) {
            zIndex = HWND_TOP;
        }
        ::SetWindowPos(WND, zIndex, m_XPos, m_YPos, m_Width, m_Height, 0);
    }

    void Win32Window::_SetWindowSize(int width, int height) {
        m_Width = width;
        m_Height = height;

        HWND zIndex = HWND_NOTOPMOST;
        if (s_InitFloating) {
            zIndex = HWND_TOP;
        }
        ::SetWindowPos(WND, zIndex, m_XPos, m_YPos, m_Width, m_Height, 0);
    }

    void Win32Window::_SetWindowTitle(const char* title) {
        if (!::SetWindowTextA(WND, title)) {
            Log::Warning("Unable to set window title: %d", GetLastError());
        } else {
            ::InvalidateRect(WND, NULL, FALSE);
        }
    }

    void Win32Window::_MakeContextCurrent() {
        if (!wglMakeCurrent(this->DC, this->RC)) {
            ShowMessage("wglMakeCurrent() failed.");
        }
    }

    void Win32Window::InitWin32(HINSTANCE hInstance) {
        Win32Window::m_HINSTANCE = hInstance;
        registerClass(hInstance);
        Win32Window::m_Initialized = true;
    }

    void Win32Window::InitWin32() {
        Win32Window::m_HINSTANCE = GetModuleHandle(NULL);
        registerClass(Win32Window::m_HINSTANCE);
        Win32Window::m_Initialized = true;

        // Proceed to create fake Win32Window...
        HWND fakeWND = CreateWindowExA(
            0,                                       // dwExStyle
            "Core", "Fake Win32Window",              // Win32Window class, Title
            WS_CLIPSIBLINGS | WS_CLIPCHILDREN,       // Style
            0, 0,                                    // Position x, y
            1, 1,                                    // Width, Height
            NULL, NULL,                              // Parent Win32Window, Menu
            m_HINSTANCE, NULL);                      // Instnce, Param

        HDC fakeDC = GetDC(fakeWND);

        PIXELFORMATDESCRIPTOR fakePFD;
        ZeroMemory(&fakePFD, sizeof(fakePFD));
        fakePFD.nSize = sizeof(fakePFD);
        fakePFD.nVersion = 1;
        fakePFD.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        fakePFD.iPixelType = PFD_TYPE_RGBA;
        fakePFD.cColorBits = 32;
        fakePFD.cAlphaBits = 8;
        fakePFD.cDepthBits = 24;

        int fakePFDID = ChoosePixelFormat(fakeDC, &fakePFD);
        if (fakePFDID == NULL) {
            ShowMessage("ChoosePixelFormat() failed.");
            //return nullptr;
        }

        if (SetPixelFormat(fakeDC, fakePFDID, &fakePFD) == false) {
            ShowMessage("SetPixelFormat() failed.");
            //return nullptr;
        }

        HGLRC fakeRC = wglCreateContext(fakeDC); // Rendering context

        if (fakeRC == NULL) {
            ShowMessage("wglCreateContext() failed.");
            //return nullptr;
        }

        if (wglMakeCurrent(fakeDC, fakeRC) == false) {
            ShowMessage("wglMakeCurrent() failed");
            //return nullptr;
        }

        wglChoosePixelFormatARB = reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(wglGetProcAddress("wglChoosePixelFormatARB"));
        if (wglChoosePixelFormatARB == nullptr) {
            ShowMessage("wglGetProcAdress() failed.");
            //return nullptr;
        }

        wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
        if (wglCreateContextAttribsARB == nullptr) {
            ShowMessage("wglGetProcAddress() failed.");
            //return nullptr;
        }

        if (!glLiteInit()) {
            ShowMessage("glLiteInit() failed.");
            //return nullptr;
        }

        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(fakeRC);
        ReleaseDC(fakeWND, fakeDC);
        DestroyWindow(fakeWND);
    }

    void* Win32Window::_GetWindowHandle() {
        return WND;
    }

    Window* Win32Window::CreateWindow(int width, int height, const char* title) {
        Log::Assert(Win32Window::m_Initialized, "You must initialize a Win32Window before creating it.");

        Win32Window* win = new Win32Window();
        win->m_Width = width;
        win->m_Height = height;

        if (s_InitAllocConsole) {
            // Allocate console Win32Window
            FreeConsole();
            bool success = AllocConsole();
            if (!success) {
                Log::Error("Could not allocate console.");
                Log::Assert(false, "%d", GetLastError());
            }
            freopen("CONOUT$", "w+", stdout);
            Log::Info("Console Win32Window created.");
        }

        DWORD winFlags = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
        if (s_InitDecorated) {
            winFlags |= WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
        }
        if (s_InitIconified) {
            winFlags |= WS_ICONIC;
        }
        if (s_InitMaximized) {
            winFlags |= WS_MAXIMIZE;
        }
        if (s_InitResizable) {
            winFlags |= WS_SIZEBOX;
        }
        if (s_InitVisible) {
            winFlags |= WS_VISIBLE;
        }
        if (s_InitAutoIconify) {
            Log::Warning("Auto Iconify flag used. But it has not been implemented yet.");
        }
        if (s_InitTransparentFramebuffer) {
            Log::Warning("Transparent framebuffer flag used. But it has not been implemented yet.");
        }
        if (s_InitHovered) {
            Log::Warning("Hovered flag used. But it has not been implemented yet.");
        }

        DWORD exWinFlags = 0;
        if (s_InitFloating) {
            exWinFlags |= WS_EX_TOPMOST;
        }

        // NOTE: Set process as DPI aware
        SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

        win->WND = CreateWindowExA(
            exWinFlags,                   // dwExStyle
            "Core", title,                // Win32Window class, Title
            winFlags,                     // Style
            CW_USEDEFAULT, CW_USEDEFAULT, // Position x, y
            width, height,                // Width, Height
            NULL, NULL,                   // Parent Win32Window, Menu
            win->m_HINSTANCE, NULL        // Instnce, Param
        );

        win->DC = GetDC(win->WND);

        if (s_InitFocused) {
            SetFocus(win->WND);
        }

        // TODO: Take into account window position for unmaximized windows
        if (s_InitCenterCursor) {
            SetCursorPos(width / 2, height / 2);
        }

        const int pixelAttribs[] = {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
            WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
            WGL_COLOR_BITS_ARB, 32,
            WGL_ALPHA_BITS_ARB, 8,
            WGL_DEPTH_BITS_ARB, 24,
            WGL_STENCIL_BITS_ARB, 8,
            WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
            WGL_SAMPLES_ARB, 4,
            0 
        };

        int pixelFormatID; UINT numFormats;
        bool status = Win32Window::wglChoosePixelFormatARB(win->DC, pixelAttribs, NULL, 1, &pixelFormatID, &numFormats);
        if (status == false || numFormats == 0) {
            ShowMessage("wglChoosePixelFormatARB() failed.");
            return nullptr;
        }

        PIXELFORMATDESCRIPTOR PFD;
        DescribePixelFormat(win->DC, pixelFormatID, sizeof(PFD), &PFD);
        SetPixelFormat(win->DC, pixelFormatID, &PFD);

        if (Win32Window::m_GlobalRC == NULL) {
            const int major_min = 4;
            const int minor_min = 5;
            int contextAttribs[] = {
                WGL_CONTEXT_MAJOR_VERSION_ARB, major_min,
                WGL_CONTEXT_MINOR_VERSION_ARB, minor_min,
                WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
                0
            };

            win->RC = wglCreateContextAttribsARB(win->DC, 0, contextAttribs);
            if (win->RC == NULL) {
                ShowMessage("wglCreateContextAttribsARB() failed.");
                return nullptr;
            }
        } else {
            win->RC = Win32Window::m_GlobalRC;
        }

        win->_MakeContextCurrent();

        wglSwapIntervalEXT = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(wglGetProcAddress("wglSwapIntervalEXT"));
        if (wglSwapIntervalEXT == nullptr) {
            ShowMessage("wglSwapIntervalEXT() not found. Cannot enable vsync.");
            return nullptr;
        }
        wglSwapIntervalEXT(Window::s_SwapInterval);

        //SetWindowTextA(win->WND, (LPCSTR(glGetString(GL_VERSION))));
        if (s_InitIconified) {
            ShowWindow(win->WND, SW_SHOWMINIMIZED);
        } else if (s_InitMaximized) {
            ShowWindow(win->WND, SW_MAXIMIZE);
        } else {
            win->_Show();
        }

        const GLubyte* vendor = glGetString(GL_VENDOR); // Returns the vendor
        const GLubyte* renderer = glGetString(GL_RENDERER); // Returns a hint to the model
        Log::Info("Renderer: %s", renderer);
        Log::Info("Vendor: %s", vendor);

        return win;
    }

    void Win32Window::UpdateSwapInterval() {
        wglSwapIntervalEXT(Window::s_SwapInterval);
    }
}

#endif