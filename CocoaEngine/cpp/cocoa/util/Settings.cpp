#include "cocoa/util/Settings.h"

namespace Cocoa
{
	namespace Settings
	{
		namespace General
		{
			// =======================================================================
			// General Settings
			// =======================================================================
			// TODO: This is a bit misleading because none of these defaults are actually ever used. See 'CocoaEditorApplication.cpp' to see how they
			// TODO: are overwritten
			extern CPath General::imGuiConfigPath = CPath::create("default.ini");
			extern CPath General::engineAssetsPath = CPath::create("assets");
			extern CPath General::engineExeDirectory = CPath::create();
			extern CPath General::engineSourceDirectory = CPath::create();
			extern CPath General::currentScene = CPath::create("New Scene.cocoa");
			extern CPath General::currentProject = CPath::create("New Project.cprj");
			extern CPath General::stylesDirectory = CPath::create("assets\\styles");
			extern CPath General::workingDirectory = CPath::create();
			extern CPath General::editorSaveData = CPath::create("EditorSaveData.json");
			extern CPath General::editorStyleData = CPath::create("EditorStyle.json");
		}

		namespace Physics2D
		{
			// =======================================================================
			// Physics Settings
			// =======================================================================
			extern int Physics2D::positionIterations = 3;
			extern int Physics2D::velocityIterations = 8;
			extern float Physics2D::timestep = 1.0f / 60.0f;
		}
	}
}