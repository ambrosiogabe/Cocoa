#include "core/LevelEditorSceneInitializer.h"

#include "renderer/Gizmos.h"
#include "core/LevelEditorSystem.h"
#include "editorWindows/InspectorWindow.h"

#include "cocoa/scenes/Scene.h"
#include "cocoa/systems/ScriptSystem.h"

// TODO: TESTING REMOVE THESE INCLUDES
#include "cocoa/renderer/fonts/Font.h"
#include "cocoa/renderer/Fonts/FontUtil.h"

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
				InspectorWindow::s_ScriptSystem = scriptSystem;
				break;
			}
		}

		InspectorWindow::ClearAllEntities();
		systems.emplace_back(std::make_unique<GizmoSystem>("Gizmo System", scene));
		systems.emplace_back(std::make_unique<LevelEditorSystem>("Level Editor System", scene, scriptSystem));

		Font testFont = Font(CPath("C:/Windows/Fonts/ariblk.ttf"));
		testFont.Load();
		testFont.Unload();
		CharInfo characterMap['z' + 1];
		FontUtil::CreateSdfFontTexture("C:/Windows/Fonts/ariblk.ttf", 32, characterMap, 'z' + 1, "sdfOutput.png", 5, 256);
	}
}
