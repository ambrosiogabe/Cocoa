#pragma once

#include "jade/core/Application.h"

extern Jade::Application* Jade::CreateApplication();

int main() {
    Jade::Application* application = Jade::CreateApplication();
    application->Run();
    delete application;
    
    return 0;
}