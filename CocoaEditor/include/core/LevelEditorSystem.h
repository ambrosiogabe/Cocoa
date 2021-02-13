#pragma once
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "cocoa/scenes/Scene.h"
#include "cocoa/events/KeyEvent.h"
#include "cocoa/events/MouseEvent.h"
#include "cocoa/scenes/SceneData.h"

namespace Cocoa
{
	namespace LevelEditorSystem
	{
		void EditorUpdate(SceneData& scene, float dt);
		void Start(SceneData& scene);
		void OnEvent(SceneData& scene, Event& e);
	};
}