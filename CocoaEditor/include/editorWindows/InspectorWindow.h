#ifndef COCOA_EDITOR_INSPECTOR_WINDOW_H
#define COCOA_EDITOR_INSPECTOR_WINDOW_H
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "cocoa/core/Entity.h"

namespace Cocoa
{
	namespace InspectorWindow
	{
		void imgui(SceneData& scene);

		void addEntity(Entity entity);

		void removeEntity(Entity entity);

		void clearAllEntities();

		Entity getActiveEntity();
	};
}

#endif
