#pragma once

#include "LevelEditorSystem.h"

#include "jade/scenes/Scene.h"
#include "jade/renderer/Shader.h"
#include "jade/renderer/Texture.h"
#include "jade/components/components.h"

namespace Jade
{
    class LevelEditorScene : public Scene
    {
    public:
        virtual void Init() override;
        virtual void Start() override;
        virtual void Render() override;
        virtual void Update(float dt) override;
        virtual void ImGui() override;

    private:
        void DoTreeNode(int& index, const Transform& transform);
        void ImGuiSceneHeirarchy();

    private:
        Shader* shader = nullptr;
        Spritesheet* sprites = nullptr;
        Texture* texture = nullptr;

        bool showDemoWindow = true;
    };
}