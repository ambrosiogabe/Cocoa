#pragma once
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "cocoa/core/Entity.h"

namespace Cocoa
{
	namespace InspectorWindow
	{
		void ImGui(SceneData& scene);

		void AddEntity(Entity entity);

		void RemoveEntity(Entity entity);

		void ClearAllEntities();

		Entity GetActiveEntity();
	};
}