#pragma once
#include "externalLibs.h"
#include "jade/components/components.h"

namespace Jade
{
    // Gizmo Components
    struct Gizmo
    {
        glm::vec2 screenPos;
        glm::vec2 screenSize;
        glm::vec4 colorTint = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    };

    class GizmoController
    {
    public:
        GizmoController();

        void Update(entt::registry registry, float dt);
        void Render();

    private:
        std::unique_ptr<Texture> m_Texture = nullptr;
        std::unique_ptr<Spritesheet> m_Spritesheet = nullptr;

        Gizmo m_VtScale;
        Gizmo m_HzScale;
        Gizmo m_FreeScale;
        
        Gizmo m_RotatePositive;
        Gizmo m_RotateNegative;
        Gizmo m_RotateFree;
        
        Gizmo m_VtMove;
        Gizmo m_HzMove;
        Gizmo m_FreeMove;
    };
}