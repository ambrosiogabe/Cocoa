#pragma once

#include "scenes/Scene.h"
#include "systems/System.h"

class TestScene : public Scene {
public:
    virtual void Init() override;
    virtual void ImGui() override;
    virtual void Render() override;
    virtual void Update(float dt) override;
    virtual void Start() override;
};