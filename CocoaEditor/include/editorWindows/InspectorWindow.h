#pragma once
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "gui/ImGuiExtended.h"

#include "cocoa/components/components.h"
#include "cocoa/components/Transform.h"
#include "cocoa/physics2d/Physics2DSystem.h"

namespace Cocoa
{
	class ScriptSystem;
	struct InspectorWindow
	{
		ScriptSystem* ScriptSystemPtr;
	};

	namespace InspectorWindowUtil
	{
		void ImGui(const InspectorWindow& inspectorWindow);

		void AddEntity(Entity entity);

		void RemoveEntity(Entity entity);

		void ClearAllEntities();

		Entity GetActiveEntity();
	};
}