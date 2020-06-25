#pragma once

#include "jade/core/Core.h"
#include "jade/scenes/Scene.h"
#include "jade/renderer/Framebuffer.h"
#include "platform/Window.h"
#include "jade/events/Event.h"

#include <string>

namespace Jade {
    class JWindow {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

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
        void SetEventCallback(const EventCallbackFn& callback);

        void SetWidth(int newWidth);
        void SetHeight(int newHeight);
        int GetWidth();
        int GetHeight();

        float GetTargetAspectRatio();
        void Render();

        void Destroy();

    private:
        JWindow() {}

    private:
        Window* m_WindowHandle;

        EventCallbackFn m_EventCallback;
        bool m_VSync = false;
        bool m_Running = true;
        int m_Width = 1920;
        int m_Height = 1080;
        float m_TargetAspectRatio = 3840.0f / 2160.0f;
    };
}