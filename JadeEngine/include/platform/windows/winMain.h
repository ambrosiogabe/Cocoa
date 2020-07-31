#pragma once

#include "jade/core/Application.h"
#include "jade/unitTests/TestMain.h"
#include "jade/util/Log.h"
#include "jade/file/IFile.h"
#include "jade/file/IFileDialog.h"
#include "jade/events/Input.h"
#include "jade/core/AssetManager.h"

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

    Jade::AssetManager::Init(0);
    Jade::IFileDialog::Init();
    Jade::IFile::Init();
    Jade::Input::Init();

    application->Run();
    delete application;
    
    Jade::IFileDialog::Destroy();
    Jade::IFile::Destroy();

    return 0;
}