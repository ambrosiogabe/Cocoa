#include "LevelEditorSceneInitializer.h"

#include "Gizmos.h"
#include "LevelEditorSystem.h"
#include "editorWindows/InspectorWindow.h"

#include "jade/scenes/Scene.h"

namespace Jade
{
	void LevelEditorSceneInitializer::Init(Scene* scene, std::vector<std::unique_ptr<System>>& systems)
	{
		InspectorWindow::ClearAllEntities();
		systems.emplace_back(std::make_unique<GizmoSystem>("Gizmo System", scene));
		systems.emplace_back(std::make_unique<LevelEditorSystem>("Level Editor System", scene));
	}
}
