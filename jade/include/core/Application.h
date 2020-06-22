#pragma once

#include "core/Core.h"
#include "core/Layer.h"
#include "platform/JWindow.h"

#include <vector>

namespace Jade {
    class JADE Application {
        public:
            Application();
            virtual ~Application();
            
            void Run();

            void PushLayer(Layer* layer);
            void PushOverlay(Layer* overlay);

            void PopLayer(Layer* layer);
            void PopOverlay(Layer* overlay);

        private:
            //std::unique_ptr<JWindow> m_Window;
            bool m_Running;

            // Overlays are always on top of layers
            // So we keep the next index for the next layer handy at all times
            std::vector<Layer*> m_Layers;
            uint32 m_LayerInsertIndex;

            float m_LastFrameTime = 0;
    };

    // To be defined in CLIENT
    Application* CreateApplication();
}