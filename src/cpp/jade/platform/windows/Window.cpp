#include "jade/platform/windows/window.h"

#include <gl/GL.h>
#include "gl/glext.h"
#include "gl/wglext.h"

#define GL_LITE_IMPLEMENTATION
#include "jade/platform/windows/GlFunctions.h"
#undef GL_LITE_IMPLEMENTATION


Window* Window::m_Instance = nullptr;

void Window::ShowMessage(LPCSTR message) {
    MessageBox(0, message, "Window::Create", MB_ICONERROR);
}

int Window::Create(HINSTANCE hInstance, int nCmdShow) {
    HWND fakeWND = CreateWindowExA(
        0,                                       // dwExStyle
        "Core", "Fake Window",                   // Window class, Title
        WS_CLIPSIBLINGS | WS_CLIPCHILDREN,       // Style
        0, 0,                                    // Position x, y
        1, 1,                                    // Width, Height
        NULL, NULL,                              // Parent Window, Menu
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

    WND = CreateWindowExA(
        0,                                                                // dwExStyle
        "Core", "OpenGL Window",                                          // Window class, Title
        WS_CAPTION | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN       // Style
            | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SIZEBOX,
        0, 0,                                                             // Position x, y
        1280, 720,                                                        // Width, Height
        NULL, NULL,                                                       // Parent Window, Menu
        hInstance, NULL                                                   // Instnce, Param
        );

    DC = GetDC(WND);

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
    bool status = wglChoosePixelFormatARB(DC, pixelAttribs, NULL, 1, &pixelFormatID, &numFormats);
    if (status == false || numFormats == 0) {
        ShowMessage("wglChoosePixelFormatARB() failed.");
        return 1;
    }

    PIXELFORMATDESCRIPTOR PFD;
    DescribePixelFormat(DC, pixelFormatID, sizeof(PFD), &PFD);
    SetPixelFormat(DC, pixelFormatID, &PFD);

    const int major_min = 4;
    const int minor_min = 5;
    int contextAttribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, major_min,
        WGL_CONTEXT_MINOR_VERSION_ARB, minor_min,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    RC = wglCreateContextAttribsARB(DC, 0, contextAttribs);
    if (RC == NULL) {
        ShowMessage("wglCreateContextAttribsARB() failed.");
        return 1;
    }

    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(fakeRC);
    ReleaseDC(fakeWND, fakeDC);
    DestroyWindow(fakeWND);
    if (!wglMakeCurrent(DC, RC)) {
        ShowMessage("wglMakeCurrent() failed.");
        return 1;
    }

    SetWindowTextA(WND, (LPCSTR(glGetString(GL_VERSION))));
    ShowWindow(WND, nCmdShow);
    Window::m_Instance = this;

    return 0;
}

void Window::Render() {
    glClearColor(0.128f, 0.586f, 0.949f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_CurrentScene->Render();

    SwapBuffers(DC);
}

void Window::Destroy() {
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

void Window::Update(float dt) {
    this->m_CurrentScene->Update(dt);
}

void Window::ChangeScene(Scene* newScene) {
    Window* win = Window::GetWindow();
    win->m_CurrentScene = newScene;
    win->m_CurrentScene->Init();
    win->m_CurrentScene->Start();
}

Window* Window::GetWindow() {
    if (Window::m_Instance == nullptr) {
        return nullptr;
    }

    return Window::m_Instance;
}