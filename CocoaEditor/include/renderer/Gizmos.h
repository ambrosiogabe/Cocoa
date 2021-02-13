#pragma once
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "cocoa/components/components.h"
#include "cocoa/physics2d/PhysicsComponents.h"
#include "cocoa/components/Transform.h"
#include "cocoa/core/Handle.h"
#include "cocoa/scenes/SceneData.h"
#include "cocoa/events/Event.h"

namespace Cocoa
{
    // Gizmo Components
    enum class GizmoMode
    {
        None,
        Translate,
        Rotate,
        Scale
    };

    enum class GizmoType
    {
        None,
        Vertical,
        Horizontal,
        Free
    };

    struct GizmoData
    {
        glm::vec3 Position;
        glm::vec3 Offset;
        glm::vec3 Tint;
        glm::vec2 HalfSize;
        glm::vec2 TexCoordMin;
        glm::vec2 TexCoordMax;

        GizmoType Type;
        Box2D Box2D;
        uint32 TextureAssetId;
        float SpriteRotation;
        bool Active;
    };

    namespace Gizmo
    {
        GizmoData CreateGizmo(const Sprite& sprite, glm::vec3 offset, float spriteRotation, GizmoType type, glm::vec3 tint={0.4f, 0.4f, 0.4f});

        void Render(const GizmoData& data, Camera* camera);
        void GizmoManipulateTranslate(const GizmoData& data, TransformData& transform, const glm::vec3& originalDragClickPos, const glm::vec3& mouseOffset, Camera* camera);
        void GizmoManipulateRotate(const GizmoData& data, TransformData& transform, const glm::vec3& startPos, const glm::vec3& mouseOffset, Camera* camera);
        void GizmoManipulateScale(const GizmoData& data, TransformData& transform, const glm::vec3& originalDragClickPos, const glm::vec3& originalScale, Camera* camera);
    };

    namespace GizmoSystem
    {
        void EditorUpdate(SceneData& scene, float dt);
        void ImGui();
        void OnEvent(SceneData& scene, Event& e);
        void Start(SceneData& scene);
    };
}