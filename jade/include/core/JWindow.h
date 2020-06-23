#pragma once

#include "core/Core.h"
#include "scenes/Scene.h"
#include "renderer/Framebuffer.h"
#include "platform/Window.h"

#include <string>

namespace Jade {
    class JWindow {
    public:
        JWindow(uint32 width, uint32 height, const std::string& name);
        virtual ~JWindow() {}

        virtual void Init(uint32 width, uint32 height, const std::string& name);
        virtual void OnUpdate();

        // Window Attributes
        virtual void SetVSync(bool enabled);
        virtual bool IsVSync() const;

        virtual void* GetNativeWindow() const;

        static JWindow* Create(uint32 width, uint32 height, const std::string& name);

        bool IsRunning();

        void SetWidth(int newWidth);
        void SetHeight(int newHeight);
        int GetWidth();
        int GetHeight();

        float GetTargetAspectRatio();
        void Render();

        static JWindow* Get();

    private:
        JWindow() {}

    private:
        static JWindow* m_Instance;
        Window* m_WindowHandle;

        bool m_VSync = false;
        bool m_Running = true;
        int m_Width = 1920;
        int m_Height = 1080;
        float m_TargetAspectRatio = 3840.0f / 2160.0f;
    };
}