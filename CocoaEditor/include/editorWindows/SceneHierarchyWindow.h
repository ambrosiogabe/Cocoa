#ifndef COCOA_EDITOR_SCENE_HIERARCHY_WINDOW_H
#define COCOA_EDITOR_SCENE_HIERARCHY_WINDOW_H
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "cocoa/scenes/SceneData.h"
#include "cocoa/core/EntityStruct.h"

namespace Cocoa
{
	namespace SceneHierarchyWindow
	{
		void init();
		void destroy();

		void addNewEntity(Entity entity);
		void imgui(SceneData& scene);
		void deleteEntity(Entity entity);

		void serialize(json& j);
		void deserialize(json& json, SceneData& scene);
	};
}

#endif
