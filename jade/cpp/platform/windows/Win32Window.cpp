#include "platform/JWindow.h"
#include "util/Log.h"
#include "events/WindowEvents.h"
#include "events/Input.h"

#include <imgui/imgui.h>
#include <gl/GL.h>
#include "gl/glext.h"
#include "gl/wglext.h"
#include <stdio.h>
#include <io.h>

#define GL_LITE_IMPLEMENTATION
#include "platform/windows/GlFunctions.h"
#undef GL_LITE_IMPLEMENTATION


void Win32Window::ShowMessage(LPCSTR message) {
    MessageBoxA(0, message, "Win32Window::Create", MB_ICONERROR);
}

int Win32Window::Create(HINSTANCE hInstance, int nCmdShow) {
    Win32Window* win = static_cast<Win32Window*>(JWindow::Get());

    // Allocate console Win32Window
    AllocConsole();
    freopen("CONOUT$", "w+", stdout);

    Log::Info("Console Win32Window created.");

    // Proceed to create Win32Window...
    HWND fakeWND = CreateWindowExA(
        0,                                       // dwExStyle
        "Core", "Fake Win32Window",                   // Win32Window class, Title
        WS_CLIPSIBLINGS | WS_CLIPCHILDREN,       // Style
        0, 0,                                    // Position x, y
        1, 1,                                    // Width, Height
        NULL, NULL,                              // Parent Win32Window, Menu
        hInstance, NULL);                        // Instnce, Param

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
        return 1;
    }

    if (SetPixelFormat(fakeDC, fakePFDID, &fakePFD) == false) {
        ShowMessage("SetPixelFormat() failed.");
        return 1;
    }

    HGLRC fakeRC = wglCreateContext(fakeDC); // Rendering context

    if (fakeRC == NULL) {
        ShowMessage("wglCreateContext() failed.");
        return 1;
    }

    if (wglMakeCurrent(fakeDC, fakeRC) == false) {
        ShowMessage("wglMakeCurrent() failed");
        return 1;
    }

    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
    wglChoosePixelFormatARB = reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(wglGetProcAddress("wglChoosePixelFormatARB"));
    if (wglChoosePixelFormatARB == nullptr) {
        ShowMessage("wglGetProcAdress() failed.");
        return 1;
    }

    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
    wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
    if (wglCreateContextAttribsARB == nullptr) {
        ShowMessage("wglGetProcAddres() failed.");
        return 1;
    }

    if (!glLiteInit()) {
        ShowMessage("glLiteInit() failed.");
        return 1;
    }

    win->WND = CreateWindowExA(
        0,                                                                // dwExStyle
        "Core", "OpenGL Win32Window",                                          // Win32Window class, Title
        WS_CAPTION | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN       // Style
            | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SIZEBOX,
        0, 0,                                                             // Position x, y
        win->m_Width, win->m_Height,                                      // Width, Height
        NULL, NULL,                                                       // Parent Win32Window, Menu
        hInstance, NULL                                                   // Instnce, Param
        );

    win->DC = GetDC(win->WND);

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
        return 1;
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
        return 1;
    }

    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(fakeRC);
    ReleaseDC(fakeWND, fakeDC);
    DestroyWindow(fakeWND);
    if (!wglMakeCurrent(win->DC, win->RC)) {
        ShowMessage("wglMakeCurrent() failed.");
        return 1;
    }

    PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(wglGetProcAddress("wglSwapIntervalEXT"));
    if (wglSwapIntervalEXT == nullptr) {
        ShowMessage("wglSwapIntervalEXT() not found. Cannot enable vsync.");
        return 1;
    }
    wglSwapIntervalEXT(1);

    SetWindowTextA(win->WND, (LPCSTR(glGetString(GL_VERSION))));
    ShowWindow(win->WND, nCmdShow);

    //win->m_ImGuiLayer.Setup(win->WND);
    win->m_WindowHandle = win->WND;

    RegisterKeyCallback(&Input::KeyCallback);
    RegisterMouseButtonCallback(&Input::MouseButtonCallback);
    RegisterCursorCallback(&Input::CursorCallback);
    RegisterScrollCallback(&Input::ScrollCallback);

    const GLubyte* vendor = glGetString(GL_VENDOR); // Returns the vendor
    const GLubyte* renderer = glGetString(GL_RENDERER); // Returns a hint to the model
    Log::Info("Renderer: %s", renderer);
    Log::Info("Vendor: %s", vendor);

    // Initialize and register Win32Window events callback
    WindowEvents::Init(win);
    RegisterResizeCallback(&WindowEvents::ResizeCallback);
    
    // Setup framebuffer
    win->m_Framebuffer = new Framebuffer(3840, 2160);

    return 0;
}

void Win32Window::_Render() {
    
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer->GetId());
    glViewport(0, 0, 3840, 2160);
    glClearColor(0.45f, 0.55f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_CurrentScene->Render();
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, GetWidth(), GetHeight());
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    m_CurrentScene->ImGui();
    SwapBuffers(DC);
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

void Win32Window::_Close() {
    PostQuitMessage(0);
}

void Win32Window::_Hide() {
    if (WND) {
        ShowWindow(WND, SW_HIDE);
    }
}

void Win32Window::_Stop() {
    _Hide();
    this->m_Running = false;
}

JWindow* Win32Window::_Create() {
    return new Win32Window();
}