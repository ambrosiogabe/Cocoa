#pragma once

#include "jade/core/Application.h"
#include "jade/unitTests/TestMain.h"
#include "jade/util/Log.h"

extern Jade::Application* Jade::CreateApplication();

int main() {
#ifdef _JADE_DEBUG
    Jade::Log::Info("Running tests!");
    if (!Jade::TestMain::Test())
    {
        return 1;
    }
#endif

    Jade::Application* application = Jade::CreateApplication();
    application->Run();
    delete application;
    
    return 0;
}