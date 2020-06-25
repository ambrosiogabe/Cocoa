#pragma once

#include "systems/System.h"
#include "components/components.h"
#include "commands/MoveTransformCommand.h"
#include "events/KeyEvent.h"
#include "events/MouseEvent.h"

namespace Jade {
    class LevelEditorSystem : public System {
    public:
        LevelEditorSystem(const char* name)
            : System(name) {}

        virtual void Update(entt::registry& registry, float dt) override;
        virtual void Start(entt::registry& registry) override;
        virtual void ImGui(entt::registry& registry) override;
        virtual void OnEvent(Event& e) override;

    private:
        void DoTreeNode(int& index, const Transform& transform, const entt::registry& registry);
        bool HandleKeyPress(KeyPressedEvent& e);
        bool HandleKeyRelease(KeyReleasedEvent& e);
        bool HandleMouseButtonPressed(MouseButtonPressedEvent& e);
        bool HandleMouseButtonReleased(MouseButtonReleasedEvent& e);

    private:
        float m_DebounceTime = 0.1f;
        float m_DebounceLeft = 0.0f;

        float m_KeyDebounceTime = 0.1f;
        float m_KeyDebounceLeft = 0.0f;

        bool m_IsDragging = false;

        glm::vec3 m_DragOffset = glm::vec3();

        glm::vec3 m_CameraSpeed = glm::vec3(0, 0, 0);
    };
}