#pragma once

#include "core/Core.h"

namespace Jade {
    class JADE Application {
        public:
            Application();
            virtual ~Application();
            
            void Run();
    };

    // To be defined in CLIENT
    Application* CreateApplication();
}