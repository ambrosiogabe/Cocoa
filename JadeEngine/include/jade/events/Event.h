#pragma once

#include "jade/core/Core.h"

#include <string>
#include <functional>

namespace Jade {

    // ================================================================
    // Event enums
    // We have to declare the first enum as a class, otherwise we get
    // a name clashing error for None = 0
    // ================================================================
    enum class EventType {
        None = 0,
        WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
        KeyPressed, KeyReleased, KeyTyped,
        MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled 
    };

    enum EventCategory {
        None = 0,
        EventCategoryApplication = 0x00000001,
        EventCategoryInput       = 0x00000010,
        EventCategoryKeyboard    = 0x00000100,
        EventCategoryMouse       = 0x00001000,
        EventCategoryMouseButton = 0x00010000
    };

    // ================================================================
    // Event helper macros
    // ================================================================
#define EVENT_CLASS_TYPE_HEADER(type) static EventType GetStaticType(); \
                                        virtual EventType GetType() const override; \
                                        virtual const char* GetName() const override;

#define EVENT_CLASS_TYPE_IMPL(type, clazz) EventType clazz::GetStaticType() { return EventType::##type; } \
                                    EventType clazz::GetType() const { return GetStaticType(); } \
                                    const char* clazz::GetName() const { return #type; }

#define EVENT_CLASS_CATEGORY_HEADER(category) int GetCategoryFlags() const;
#define EVENT_CLASS_CATEGORY_IMPL(category, clazz) int clazz::GetCategoryFlags() const { return category; }

    // ================================================================
    // Event Base Class
    // ================================================================
    class Event {
        friend class EventDispatcher;

    public:
        virtual EventType GetType() const = 0;
        virtual const char* GetName() const = 0;
        virtual int GetCategoryFlags() const = 0;
        virtual std::string ToString() const { return GetName(); }

        inline bool IsInCategory(EventCategory category);
        inline bool Handled();
    
    protected:
        bool m_Handled = false;
    };

    // ================================================================
    // Event dispatcher
    // ================================================================
    class EventDispatcher {
        template<typename T>
        using EventFn = std::function<bool(T&)>;

    public:
        EventDispatcher(Event& event);

        template<typename T>
        bool Dispatch(EventFn<T> function) {
            if (m_Event.GetType() == T::GetStaticType()) {
                m_Event.m_Handled = function(*(T*)&m_Event);
                return true;
            }

            return false;
        }

    private:
        Event& m_Event;
    };

    inline std::ostream& operator<<(std::ostream& os, const Event& e) {
        return os << e.ToString();
    }
}