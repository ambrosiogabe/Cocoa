#include "externalLibs.h"

#include "jade/events/WindowEvent.h"

namespace Jade {
    WindowResizeEvent::WindowResizeEvent(uint32 width, uint32 height)
        : m_Width(width), m_Height(height) {}

    uint32 WindowResizeEvent::GetWidth() const {
        return m_Width;
    }

    uint32 WindowResizeEvent::GetHeight() const {
        return m_Height;
    }

    std::string WindowResizeEvent::ToString() const {
        std::stringstream ss;
        ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
        return ss.str();
    }

    EVENT_CLASS_TYPE_IMPL(WindowResize, WindowResizeEvent)
    EVENT_CLASS_CATEGORY_IMPL(EventCategoryApplication, WindowResizeEvent)

    WindowCloseEvent::WindowCloseEvent() {

    }

    EVENT_CLASS_TYPE_IMPL(WindowClose, WindowCloseEvent)
    EVENT_CLASS_CATEGORY_IMPL(EventCategoryApplication, WindowCloseEvent)
}