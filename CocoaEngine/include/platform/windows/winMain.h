#pragma once

#include "cocoa/core/Application.h"

extern Cocoa::Application* Cocoa::CreateApplication();

int main() {

    Cocoa::Application* application = Cocoa::CreateApplication();

    application->Init();

    application->Run();

    application->Shutdown();

    delete application;

    return 0;
}