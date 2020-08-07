#pragma once
#include "externalLibs.h"

#include "jade/events/Event.h"

namespace Jade {
    
    class Scene;

    class Layer {
    public:
        Layer(Scene* scene, const std::string& name = "Layer")
            : m_Scene(scene), m_DebugName(name) {}
        virtual ~Layer() {}

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate(float dt) {}
        virtual void OnRender() {}

        virtual void OnEvent(Event& e) {}

    protected:
        std::string m_DebugName;
        Scene* m_Scene = nullptr;
    };
}