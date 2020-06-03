#pragma once

#include "jade/scenes/Scene.h"

class TestScene : public Scene {
public:
    virtual void Init() override;
    virtual void Render() override;
    virtual void Update(float dt) override;
    virtual void Start() override;
};