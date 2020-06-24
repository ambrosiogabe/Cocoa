#pragma once

#include "core/Core.h"
#include "events/Event.h"

namespace Jade {
    class WindowResizeEvent : public Event {
    public:
        WindowResizeEvent(uint32 width, uint32 height);

        inline uint32 GetWidth() const;
        inline uint32 GetHeight() const;

        virtual std::string ToString() const override;

        EVENT_CLASS_TYPE_HEADER(WindowResize)
        EVENT_CLASS_CATEGORY_HEADER(EventCategoryApplication)

    private:
        uint32 m_Width;
        uint32 m_Height;
    };

    class WindowCloseEvent : public Event {
    public:
        WindowCloseEvent();

        EVENT_CLASS_TYPE_HEADER(WindowClose)
        EVENT_CLASS_CATEGORY_HEADER(EventCategoryApplication)
    };
}