#pragma once

#include "events/Event.h"

#include <string>

namespace Jade {
    class Layer {
    public:
        Layer(const std::string& name = "Layer")
            : m_DebugName(name) {}
        virtual ~Layer() {}

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate(float dt) {}
        virtual void OnImGuiRender() {}
        virtual void OnRender() {}

        virtual void OnEvent(Event& e) {}

    protected:
        std::string m_DebugName;
    };
}