#pragma once

#include "jade/core/Application.h"

// test includes
#include "platform/Window.h"
#include "jade/util/Log.h"
#include "jade/core/Core.h"
#include "stb/stb_image.h"

extern Jade::Application* Jade::CreateApplication();

int main() {
    // Jade::Application* application = Jade::CreateApplication();
    // application->Run();
    // delete application;

    int width;
    int height;
    int channels;
    Jade::uint32* bytes = (Jade::uint32*)stbi_load("assets/images/icon.png", &width, &height, &channels, 4);

    Jade::WindowImage windowImage = {
        width, height,
        bytes
    };

    Jade::Window::Init();
    Jade::Window::SwapInterval(1);
    Jade::Window* win1 = Jade::Window::CreateWindow(1920, 1080, "Some Title");
    Jade::Window* win2 = Jade::Window::CreateWindow(1280, 720, "Window 2");
    Jade::Window* win3 = Jade::Window::CreateWindow(300, 300, "Another Window!!");

    Jade::Window::SetWindowIcon(win1, 1, &windowImage);

    Jade::Window::SetWindowTitle(win1, "Something else");
    float lastFrameTime = Jade::Window::GetTime();
    bool shouldClose = false;
    while (!shouldClose) {
        float time = Jade::Window::GetTime();
        float dt = time - lastFrameTime;
        lastFrameTime = time;

        if (Jade::Window::WindowShouldClose(win1)) {
            Jade::Window::Destroy(win1);
            shouldClose = Jade::Window::WindowShouldClose(win1);// && Jade::Window::WindowShouldClose(win2) && Jade::Window::WindowShouldClose(win3);
        } else {
            Jade::Window::MakeContextCurrent(win1);
            glClearColor(1, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT);
        
            char buf[128] = {};
            snprintf(buf, sizeof(buf), "%2.4f", 1.0f/dt);
            Jade::Window::SetWindowTitle(win1, buf);
            Jade::Window::SwapBuffers(win1);
        }

        if (Jade::Window::WindowShouldClose(win2)) {
            Jade::Window::Destroy(win2);
            shouldClose = Jade::Window::WindowShouldClose(win1) && Jade::Window::WindowShouldClose(win2) && Jade::Window::WindowShouldClose(win3);
        } else {
            Jade::Window::MakeContextCurrent(win2);
            glClearColor(1, 1, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT);
            Jade::Window::SwapBuffers(win2);
        }

        if (Jade::Window::WindowShouldClose(win3)) {
            Jade::Window::Destroy(win3);
            shouldClose = Jade::Window::WindowShouldClose(win1) && Jade::Window::WindowShouldClose(win2) && Jade::Window::WindowShouldClose(win3);
        } else {
            Jade::Window::MakeContextCurrent(win3);
            glClearColor(0, 1, 1, 1);
            glClear(GL_COLOR_BUFFER_BIT);
            Jade::Window::SwapBuffers(win3);
        }

        Jade::Window::PollEvents();
    }

    // Jade::Window::Destroy(win1);
    // Jade::Window::Destroy(win2);
    // Jade::Window::Destroy(win3);
    return 0;
}