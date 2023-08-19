#ifndef COCOA_EDITOR_GIZMOS_H
#define COCOA_EDITOR_GIZMOS_H
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "cocoa/components/Transform.h"
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
		glm::vec2 position;
		glm::vec3 offset;
		glm::vec3 color;
		glm::vec2 scale;
		glm::vec2 boxBoundsHalfSize;
		glm::vec2 boxBoundsOffset;
		float rotation;

		GizmoType type;
		bool active;
	};

	namespace Gizmo
	{
		GizmoData createGizmo(
			glm::vec3& offset,
			float rotation,
			GizmoType type,
			glm::vec3& color,
			glm::vec2& boxBoundsHalfSize,
			glm::vec2& boxBoundsOffset = glm::vec2{ 0.0f, 0.0f },
			glm::vec2& scale = glm::vec2{ 1.0f, 1.0f });

		void render(const GizmoData& data, const Camera& camera, GizmoMode mode);
		void gizmoManipulateTranslate(const GizmoData& data, TransformData& transform, const glm::vec3& originalDragClickPos, const glm::vec3& mouseOffset, const Camera& camera);
		void gizmoManipulateRotate(const GizmoData& data, TransformData& transform, const glm::vec3& startPos, const glm::vec3& mouseOffset, const Camera& camera);
		void gizmoManipulateScale(const GizmoData& data, TransformData& transform, const glm::vec3& originalDragClickPos, const glm::vec3& originalScale, const Camera& camera);
	};

	namespace GizmoSystem
	{
		void editorUpdate(SceneData& scene, float dt);
		void imgui();
		void onEvent(SceneData& scene, Event& e);
		void init(SceneData& scene);
		void destroy(SceneData& scene);
	};
}

#endif
