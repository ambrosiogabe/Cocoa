#pragma once
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "cocoa/scenes/Scene.h"
#include "cocoa/systems/System.h"
#include "cocoa/events/KeyEvent.h"
#include "cocoa/events/MouseEvent.h"

namespace Cocoa
{
	namespace LevelEditorSystem
	{
		void EditorUpdate(Scene* scene, float dt);
		void Start(Scene* scene);
		void OnEvent(Scene* scene, Event& e);
	};
}