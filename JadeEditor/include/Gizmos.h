#pragma once
#include "externalLibs.h"
#include "jade/components/components.h"
#include "jade/components/Transform.h"
#include "jade/systems/System.h"

namespace Jade
{
    // Gizmo Components
    enum class GizmoMode
    {
        None,
        Translate,
        Rotate,
        Scale
    };

    class Gizmo
    {
    public:
        Gizmo(Sprite* sprite, glm::vec3 startPosition, glm::vec2 halfSize, float spriteRotation);

        inline bool GizmoIsActive() { return m_Active; }
        void Render();
        void GizmoManipulateTranslate(Transform& transform);
        void GizmoManipulateRotate(Transform& transform);
        void GizmoManipulateScale(Transform& transform);

    public:
        glm::vec3 m_Position;
        glm::vec2 m_HalfSize;
        glm::vec2 m_TexCoordMin;
        glm::vec2 m_TexCoordMax;
        uint32 m_TexId;
        float m_SpriteRotation;
        bool m_Active;
        bool m_Visible = false;
    };

    class GizmoSystem : public System
    {
    public:
        GizmoSystem(const char* name="Gizmo System");

        void Update(const entt::registry& registry, float dt);
        virtual void OnEvent(Event& e) override;

    private:
        bool HandleKeyPress(KeyPressedEvent& e);
        bool HandleKeyRelease(KeyReleasedEvent& e);
        bool HandleMouseButtonPressed(MouseButtonPressedEvent& e);
        bool HandleMouseButtonReleased(MouseButtonReleasedEvent& e);
        bool HandleMouseScroll(MouseScrolledEvent& e);

    private:
        std::unique_ptr<Texture> m_Texture = nullptr;
        std::unique_ptr<Spritesheet> m_Spritesheet = nullptr;

        int m_ActiveGizmo = -1;
        GizmoMode m_Mode = GizmoMode::Translate;

        union
        {
            Gizmo m_Gizmos[6];
            struct
            {
                Gizmo m_VtScale;
                Gizmo m_HzScale;
                Gizmo m_FreeScale;

                Gizmo m_VtMove;
                Gizmo m_HzMove;
                Gizmo m_FreeMove;

                //Gizmo m_RotatePositive;
                //Gizmo m_RotateNegative;
                //Gizmo m_RotateFree;
            };
        };
    };
}