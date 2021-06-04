#pragma once

#define GABE_CPP_UTILS_IMPL
#include <CppUtils/CppUtils.h>
#undef GABE_CPP_UTILS_IMPL

#include "cocoa/core/Application.h"

extern Cocoa::Application* Cocoa::CreateApplication();

int main() 
{
    Cocoa::Application* application = Cocoa::CreateApplication();

    application->Init();

    application->Run();

    application->Shutdown();

    delete application;
    return 0;
}