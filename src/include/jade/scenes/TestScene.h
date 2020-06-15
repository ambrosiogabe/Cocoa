#pragma once

#include "jade/scenes/Scene.h"
#include "jade/systems/System.h"

class TestScene : public Scene {
public:
    virtual void Init() override;
    virtual void ImGui() override;
    virtual void Render() override;
    virtual void Update(float dt) override;
    virtual void Start() override;

private:
    System* m_Systems[100];
};