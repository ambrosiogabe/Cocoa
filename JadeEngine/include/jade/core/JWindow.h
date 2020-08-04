#pragma once
#include "externalLibs.h"

#include "jade/core/Core.h"
#include "jade/scenes/Scene.h"
#include "jade/renderer/Framebuffer.h"
#include "jade/events/Event.h"

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
        glm::vec2 GetWindowPos();

        void SetWidth(int newWidth) { m_Size.x = (float)newWidth; }
        void SetHeight(int newHeight) { m_Size.y = (float)newHeight; }
        void SetSize(const glm::vec2& size);
        void SetTitle(const char* newTitle);
        glm::vec2 GetMonitorSize();
        int GetWidth() { return (int)m_Size.x; }
        int GetHeight() { return (int)m_Size.y; }
        const glm::vec2& GetSize() { return m_Size; }

        float GetTargetAspectRatio() { return m_TargetAspectRatio; }
        void Render();

        void Destroy();

    private:
        JWindow() {}

    private:
        void* m_WindowHandle = nullptr;

        EventCallbackFn m_EventCallback = nullptr;
        bool m_VSync = false;
        bool m_Running = true;
        glm::vec2 m_Size{ 1920, 1080 };
        float m_TargetAspectRatio = 3840.0f / 2160.0f;
    };
}