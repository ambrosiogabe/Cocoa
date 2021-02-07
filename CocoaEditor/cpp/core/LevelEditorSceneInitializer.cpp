#include "core/LevelEditorSceneInitializer.h"

#include "renderer/Gizmos.h"
#include "editorWindows/InspectorWindow.h"
#include "core/LevelEditorSystem.h"
#include "core/ImGuiLayer.h"
#include "core/CocoaEditorApplication.h"

#include "cocoa/scenes/Scene.h"
#include "cocoa/systems/ScriptSystem.h"

namespace Cocoa
{
	void LevelEditorSceneInitializer::Init(Scene* scene)
	{
		InspectorWindow::ClearAllEntities();
	}

	void LevelEditorSceneInitializer::Start(Scene* scene)
	{
		GizmoSystem::Start(scene);
		LevelEditorSystem::Start(scene);
	}
}
