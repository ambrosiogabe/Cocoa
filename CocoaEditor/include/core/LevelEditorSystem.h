#ifndef COCOA_EDITOR_LEVEL_EDITOR_SYSTEM_H
#define COCOA_EDITOR_LEVEL_EDITOR_SYSTEM_H
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "cocoa/scenes/Scene.h"
#include "cocoa/scenes/SceneData.h"

namespace Cocoa
{
	namespace LevelEditorSystem
	{
		void editorUpdate(SceneData& scene, float dt);
		void init(SceneData& scene);
		void onEvent(SceneData& scene, Event& e);
		void destroy(SceneData& scene);

		Camera& getCamera();

		void serialize(json& j);
		void deserialize(const json& j, SceneData& scene);
	};
}

#endif
