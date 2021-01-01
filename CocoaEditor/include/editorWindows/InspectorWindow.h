#pragma once

#include "Gui/ImGuiExtended.h"

#include "cocoa/components/components.h"
#include "cocoa/components/Transform.h"
#include "cocoa/physics2d/Physics2DSystem.h"

namespace Cocoa
{
	class ScriptSystem;
	class InspectorWindow
	{
	public:
		static void ImGui();
		static void AddEntity(Entity entity);
		static void RemoveEntity(Entity entity);
		static void ClearAllEntities();
		static Entity GetActiveEntity();

		// =====================================================================
		// Script components
		// =====================================================================
		static void ImGuiScriptComponents(ScriptSystem* scriptSystem);

		// =====================================================================
		// Basic components
		// =====================================================================
		static void ImGuiTransform(Transform& transform);

		// =====================================================================
		// Renderer components
		// =====================================================================
		static void ImGuiSpriteRenderer(SpriteRenderer& spr);

		// =====================================================================
		// Physics components
		// =====================================================================
		static void ImGuiRigidbody2D(Rigidbody2D& rb);
		static void ImGuiAABB(AABB& box);
		static void ImGuiBox2D(Box2D& box);
		static void ImGuiCircle(Circle& circle);

	private:
		static void ImGuiAddComponentButton();

	public:
		static ScriptSystem* s_ScriptSystem;

	private:
		static std::vector<Entity> s_ActiveEntities;
	};
}