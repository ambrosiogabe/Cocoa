#include "events/Event.h"

namespace Jade {
    EventDispatcher::EventDispatcher(Event& e) 
        : m_Event(e) {}
        
    bool Event::Handled() {
        return m_Handled;
    }
}