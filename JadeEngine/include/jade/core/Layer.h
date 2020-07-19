#pragma once
#include "externalLibs.h"

#include "jade/events/Event.h"

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