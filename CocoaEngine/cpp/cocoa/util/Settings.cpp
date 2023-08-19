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
			extern std::filesystem::path imGuiConfigPath = "default.ini";
			extern std::filesystem::path engineAssetsPath = "assets";
			extern std::filesystem::path engineExeDirectory = "";
			extern std::filesystem::path engineSourceDirectory = "";
			extern std::filesystem::path currentScene = "New Scene.cocoa";
			extern std::filesystem::path currentProject = "New Project.cprj";
			extern std::filesystem::path stylesDirectory = "assets/styles";
			extern std::filesystem::path workingDirectory = "";
			extern std::filesystem::path editorSaveData = "EditorSaveData.json";
			extern std::filesystem::path editorStyleData = "EditorStyle.json";
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