#pragma once

#include "cocoa/core/Application.h"
#include "cocoa/util/Log.h"
#include "cocoa/file/IFile.h"
#include "cocoa/file/FileDialog.h"
#include "cocoa/events/Input.h"
#include "cocoa/core/AssetManager.h"

#include "cocoa/util/DynamicArray.h"
#include "cocoa/util/Log.h"

#include <Windows.h>

extern Cocoa::Application* Cocoa::CreateApplication();

int main() {
    Cocoa::Application* application = Cocoa::CreateApplication();

    application->Init();

    application->Run();

    application->Shutdown();
    
    delete application;
    return 0;
}