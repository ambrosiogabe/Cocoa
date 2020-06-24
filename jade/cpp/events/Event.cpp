#include "events/Event.h"

namespace Jade {
    EventDispatcher::EventDispatcher(Event& e) 
        : m_Event(e) {}

    template<typename T>
    bool EventDispatcher::Dispatch(EventFn<T> function) {
        if (m_Event.GetType() == T::GetStaticType()) {
            m_Event.m_Handled = function(*(T*)&m_Event);
            return true;
        }

        return false;
    }

    bool Event::Handled() {
        return m_Handled;
    }
}