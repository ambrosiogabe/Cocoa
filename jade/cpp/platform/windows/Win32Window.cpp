#include "platform/Window.h"
#include "util/Log.h"
#include "events/WindowEvents.h"
#include "events/Input.h"
#include "renderer/DebugDraw.h"

#include <imgui/imgui.h>
#include <gl/GL.h>
#include "gl/glext.h"
#include "gl/wglext.h"
#include <stdio.h>
#include <io.h>

#define GL_LITE_IMPLEMENTATION
#include "platform/windows/GlFunctions.h"
#undef GL_LITE_IMPLEMENTATION

namespace Jade {
    bool Win32Window::m_Initialized = false;
    HINSTANCE Win32Window::m_HINSTANCE = {};

    void Win32Window::DefaultResizeCallback(HWND wnd, int width, int height) {
        for (Window* window : m_Windows) {
            Win32Window* win = static_cast<Win32Window*>(window);
            if (win->WND == wnd) {
                if (window->m_ResizeCallback != nullptr) {
                    window->m_ResizeCallback(window, width, height);
                }
                break;
            }
        }
    }

    void Win32Window::DefaultMouseButtonCallback(HWND wnd, int button, int action, int mods) {
        for (Window* window : m_Windows) {
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
        for (Window* window : m_Windows) {
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
        for (Window* window : m_Windows) {
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
        for (Window* window : m_Windows) {
            Win32Window* win = static_cast<Win32Window*>(window);
            if (win->WND == wnd) {
                if (window->m_KeyCallback != nullptr) {
                    window->m_KeyCallback(window, key, scancode, action, mods);
                }
                break;
            }
        }
    }

    void Win32Window::ShowMessage(LPCSTR message) {
        MessageBoxA(0, message, "Win32Window::Create", MB_ICONERROR);
    }

    static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        LRESULT result = DefWindowProcA(hWnd, message, wParam, lParam);

        switch (message) {
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

    static ATOM registerClass(HINSTANCE hInstance) {
        WNDCLASSEXA wcex;
        ZeroMemory(&wcex, sizeof(wcex));
        wcex.cbSize = sizeof(wcex);
        wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wcex.lpfnWndProc = WindowProcedure;
        wcex.hInstance = hInstance;
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex.lpszClassName = "Core";

        return RegisterClassExA(&wcex);
    }

    // void Win32Window::_Render() {
        
    //     glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer->GetId());
    //     glViewport(0, 0, 3840, 2160);
    //     glClearColor(0.45f, 0.55f, 0.6f, 1.0f);
    //     glClear(GL_COLOR_BUFFER_BIT);

    //     DebugDraw::EndFrame();
    //     m_CurrentScene->Render();
        
    //     glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //     glViewport(0, 0, GetWidth(), GetHeight());
    //     glClearColor(0, 0, 0, 1);
    //     glClear(GL_COLOR_BUFFER_BIT);

    //     m_CurrentScene->ImGui();
    //     SwapBuffers(DC);
    // }

    void Win32Window::SwapBuffers() {
        ::SwapBuffers(DC);
    }

    void Win32Window::Destroy() {
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

    void Win32Window::Hide() {
        if (WND) {
            ShowWindow(WND, SW_HIDE);
        }
    }

    void Win32Window::Show() {
        ShowWindow(WND, SW_SHOWNORMAL);
        if (m_InitFocusOnShow) {
            SetFocus(WND);
        }
    }

    void Win32Window::PollEvents() {
        MSG msg = {0, 0, 0, 0, 0, NULL};
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                this->Destroy();
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    void Win32Window::InitWin32(HINSTANCE hInstance) {
        Win32Window::m_HINSTANCE = hInstance;
        registerClass(hInstance);
        Win32Window::m_Initialized = true;
    }

    Window* Win32Window::CreateWindow(int width, int height, const char* title) {
        Log::Assert(Win32Window::m_Initialized, "You must initialize a Win32Window before creating it.");

        Win32Window* win = new Win32Window();

        if (win->m_InitAllocConsole) {
            // Allocate console Win32Window
            AllocConsole();
            freopen("CONOUT$", "w+", stdout);
            Log::Info("Console Win32Window created.");
        }

        // Proceed to create fake Win32Window...
        HWND fakeWND = CreateWindowExA(
            0,                                       // dwExStyle
            "Core", "Fake Win32Window",              // Win32Window class, Title
            WS_CLIPSIBLINGS | WS_CLIPCHILDREN,       // Style
            0, 0,                                    // Position x, y
            1, 1,                                    // Width, Height
            NULL, NULL,                              // Parent Win32Window, Menu
            win->m_HINSTANCE, NULL);                 // Instnce, Param

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
            return nullptr;
        }

        if (SetPixelFormat(fakeDC, fakePFDID, &fakePFD) == false) {
            ShowMessage("SetPixelFormat() failed.");
            return nullptr;
        }

        HGLRC fakeRC = wglCreateContext(fakeDC); // Rendering context

        if (fakeRC == NULL) {
            ShowMessage("wglCreateContext() failed.");
            return nullptr;
        }

        if (wglMakeCurrent(fakeDC, fakeRC) == false) {
            ShowMessage("wglMakeCurrent() failed");
            return nullptr;
        }

        PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
        wglChoosePixelFormatARB = reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(wglGetProcAddress("wglChoosePixelFormatARB"));
        if (wglChoosePixelFormatARB == nullptr) {
            ShowMessage("wglGetProcAdress() failed.");
            return nullptr;
        }

        PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
        wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
        if (wglCreateContextAttribsARB == nullptr) {
            ShowMessage("wglGetProcAddres() failed.");
            return nullptr;
        }

        if (!glLiteInit()) {
            ShowMessage("glLiteInit() failed.");
            return nullptr;
        }

        DWORD winFlags = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
        if (win->m_InitDecorated) {
            winFlags |= WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
        }
        if (win->m_InitIconified) {
            winFlags |= WS_ICONIC;
        }
        if (win->m_InitMaximized) {
            winFlags |= WS_MAXIMIZE;
        }
        if (win->m_InitResizable) {
            winFlags |= WS_SIZEBOX;
        }
        if (win->m_InitAutoIconify) {
            Log::Warning("Auto Iconify flag used. But it has not been implemented yet.");
        }
        if (win->m_InitTransparentFramebuffer) {
            Log::Warning("Transparent framebuffer flag used. But it has not been implemented yet.");
        }
        if (win->m_InitHovered) {
            Log::Warning("Hovered flag used. But it has not been implemented yet.");
        }

        DWORD exWinFlags = 0;
        if (win->m_InitFloating) {
            exWinFlags |= WS_EX_TOPMOST;
        }

        win->WND = CreateWindowExA(
            exWinFlags,                   // dwExStyle
            "Core", title,                // Win32Window class, Title
            winFlags,                     // Style
            0, 0,                         // Position x, y
            width, height,                // Width, Height
            NULL, NULL,                   // Parent Win32Window, Menu
            win->m_HINSTANCE, NULL        // Instnce, Param
        );

        win->DC = GetDC(win->WND);

        if (win->m_InitFocused) {
            SetFocus(win->WND );
        }

        // TODO: Take into account window position for unmaximized windows
        if (win->m_InitCenterCursor) {
            SetCursorPos(width / 2, height / 2);
        }

        // NOTE: Set process as DPI aware
        SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

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
        bool status = wglChoosePixelFormatARB(win->DC, pixelAttribs, NULL, 1, &pixelFormatID, &numFormats);
        if (status == false || numFormats == 0) {
            ShowMessage("wglChoosePixelFormatARB() failed.");
            return nullptr;
        }

        PIXELFORMATDESCRIPTOR PFD;
        DescribePixelFormat(win->DC, pixelFormatID, sizeof(PFD), &PFD);
        SetPixelFormat(win->DC, pixelFormatID, &PFD);

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

        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(fakeRC);
        ReleaseDC(fakeWND, fakeDC);
        DestroyWindow(fakeWND);
        if (!wglMakeCurrent(win->DC, win->RC)) {
            ShowMessage("wglMakeCurrent() failed.");
            return nullptr;
        }

        PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(wglGetProcAddress("wglSwapIntervalEXT"));
        if (wglSwapIntervalEXT == nullptr) {
            ShowMessage("wglSwapIntervalEXT() not found. Cannot enable vsync.");
            return nullptr;
        }
        wglSwapIntervalEXT(1);

        //SetWindowTextA(win->WND, (LPCSTR(glGetString(GL_VERSION))));
        if (win->m_InitIconified) {
            ShowWindow(win->WND, SW_SHOWMINIMIZED);
        } else if (win->m_InitMaximized) {
            ShowWindow(win->WND, SW_MAXIMIZE);
        } else {
            win->Show();
        }

        //win->m_ImGuiLayer.Setup(win->WND);
        //win->m_WindowHandle = win->WND;

        //RegisterKeyCallback(&Input::KeyCallback);
        //RegisterMouseButtonCallback(&Input::MouseButtonCallback);
        //RegisterCursorCallback(&Input::CursorCallback);
        //RegisterScrollCallback(&Input::ScrollCallback);

        const GLubyte* vendor = glGetString(GL_VENDOR); // Returns the vendor
        const GLubyte* renderer = glGetString(GL_RENDERER); // Returns a hint to the model
        Log::Info("Renderer: %s", renderer);
        Log::Info("Vendor: %s", vendor);

        // Initialize and register Win32Window events callback
        //WindowEvents::Init(win);
        //RegisterResizeCallback(&WindowEvents::ResizeCallback);
        
        // Setup framebuffer
        //win->m_Framebuffer = new Framebuffer(3840, 2160);

        return win;
    }
}