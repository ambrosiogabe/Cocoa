#pragma once

#include <string>

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

protected:
    std::string m_DebugName;
};