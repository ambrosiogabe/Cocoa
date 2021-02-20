#pragma once
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "cocoa/physics2d/PhysicsComponents.h"
#include "cocoa/components/Sprite.h"
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
		glm::vec2 Position;
		glm::vec3 Offset;
		glm::vec3 Color;
		glm::vec2 Scale;
		glm::vec2 BoxBoundsHalfSize;
		glm::vec2 BoxBoundsOffset;
		float Rotation;

		GizmoType Type;
		bool Active;
	};

	namespace Gizmo
	{
		GizmoData CreateGizmo(
			glm::vec3& offset,
			float rotation,
			GizmoType type,
			glm::vec3& color,
			glm::vec2& boxBoundsHalfSize,
			glm::vec2& boxBoundsOffset = glm::vec2{ 0.0f, 0.0f },
			glm::vec2& scale = glm::vec2{ 1.0f, 1.0f });

		void Render(const GizmoData& data, const Camera& camera, GizmoMode mode);
		void GizmoManipulateTranslate(const GizmoData& data, TransformData& transform, const glm::vec3& originalDragClickPos, const glm::vec3& mouseOffset, const Camera& camera);
		void GizmoManipulateRotate(const GizmoData& data, TransformData& transform, const glm::vec3& startPos, const glm::vec3& mouseOffset, const Camera& camera);
		void GizmoManipulateScale(const GizmoData& data, TransformData& transform, const glm::vec3& originalDragClickPos, const glm::vec3& originalScale, const Camera& camera);
	};

	namespace GizmoSystem
	{
		void EditorUpdate(SceneData& scene, float dt);
		void ImGui();
		void OnEvent(SceneData& scene, Event& e);
		void Init(SceneData& scene);
		void Destroy(SceneData& scene);
	};
}