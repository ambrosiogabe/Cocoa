#pragma once

#include "cocoa/core/Application.h"
#include "cocoa/unitTests/TestMain.h"
#include "cocoa/util/Log.h"
#include "cocoa/file/IFile.h"
#include "cocoa/file/IFileDialog.h"
#include "cocoa/events/Input.h"
#include "cocoa/core/AssetManager.h"

#include <Windows.h>

extern Cocoa::Application* Cocoa::CreateApplication();

int main() {
#ifdef _COCOA_DEBUG
    Cocoa::Log::Info("Running tests!");
    if (!Cocoa::TestMain::Test())
    {
        return 1;
    }
#endif

    Cocoa::Application* application = Cocoa::CreateApplication();

    application->Init();

    application->Run();

    application->Shutdown();
    
    delete application;
    return 0;
}