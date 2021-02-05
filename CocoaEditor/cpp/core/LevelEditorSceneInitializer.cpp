#include "core/LevelEditorSceneInitializer.h"

#include "renderer/Gizmos.h"
#include "core/LevelEditorSystem.h"
#include "editorWindows/InspectorWindow.h"

#include "cocoa/scenes/Scene.h"
#include "cocoa/systems/ScriptSystem.h"

namespace Cocoa
{
	void LevelEditorSceneInitializer::Init(Scene* scene, std::vector<std::unique_ptr<System>>& systems)
	{
		ScriptSystem* scriptSystem = nullptr;
		for (const auto& system : systems)
		{
			if (strcmp(system->GetName(), "Script System") == 0)
			{
				scriptSystem = (ScriptSystem*)system.get();
				break;
			}
		}

		InspectorWindowUtil::ClearAllEntities();
		systems.emplace_back(std::make_unique<GizmoSystem>("Gizmo System", scene));
		systems.emplace_back(std::make_unique<LevelEditorSystem>("Level Editor System", scene, scriptSystem));
	}
}
