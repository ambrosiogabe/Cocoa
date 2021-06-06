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
			extern PathBuilder imGuiConfigPath = "default.ini";
			extern PathBuilder engineAssetsPath = "assets";
			extern PathBuilder engineExeDirectory = PathBuilder();
			extern PathBuilder engineSourceDirectory = PathBuilder();
			extern PathBuilder currentScene = "New Scene.cocoa";
			extern PathBuilder currentProject = "New Project.cprj";
			extern PathBuilder stylesDirectory = "assets/styles";
			extern PathBuilder workingDirectory = PathBuilder();
			extern PathBuilder editorSaveData = "EditorSaveData.json";
			extern PathBuilder editorStyleData = "EditorStyle.json";
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