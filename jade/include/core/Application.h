#pragma once

#include "core/Core.h"
#include "core/Layer.h"
#include "core/JWindow.h"
#include "events/Event.h"
#include "events/WindowEvent.h"

#include <vector>

namespace Jade {
    class Application {
        public:
            Application();
            virtual ~Application();
            
            void Run();
            void Stop();

            void PushLayer(Layer* layer);
            void PushOverlay(Layer* overlay);

            void PopLayer(Layer* layer);
            void PopOverlay(Layer* overlay);

            virtual void OnEvent(Event& e);

            Framebuffer* GetFramebuffer() const;
            void ChangeScene(Scene* scene);
            Scene* GetScene() const;

            static Application* Get();

        private:
            bool OnWindowClose(WindowCloseEvent& e);

            static Application* s_Instance;

            JWindow* m_Window;
            bool m_Running;

            // Overlays are always on top of layers
            // So we keep the next index for the next layer handy at all times
            std::vector<Layer*> m_Layers;
            uint32 m_LayerInsertIndex;

            float m_LastFrameTime = 0;

        protected:
            Framebuffer* m_Framebuffer = nullptr;
            Scene* m_CurrentScene = nullptr;
    };

    // To be defined in CLIENT
    Application* CreateApplication();
}