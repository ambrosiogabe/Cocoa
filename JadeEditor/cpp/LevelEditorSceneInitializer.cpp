#include "LevelEditorSceneInitializer.h"

#include "Gizmos.h"
#include "LevelEditorSystem.h"
#include "editorWindows/InspectorWindow.h"

#include "jade/scenes/Scene.h"
#include "jade/systems/ScriptSystem.h"

namespace Jade
{
	void LevelEditorSceneInitializer::Init(Scene* scene, std::vector<std::unique_ptr<System>>& systems)
	{
		ScriptSystem* scriptSystem = nullptr;
		for (const auto& system : systems)
		{
			if (strcmp(system->GetName(), "Script System") == 0)
			{
				scriptSystem = (ScriptSystem*)system.get();
				InspectorWindow::s_ScriptSystem = scriptSystem;
				break;
			}
		}

		InspectorWindow::ClearAllEntities();
		systems.emplace_back(std::make_unique<GizmoSystem>("Gizmo System", scene));
		systems.emplace_back(std::make_unique<LevelEditorSystem>("Level Editor System", scene, scriptSystem));
	}
}
