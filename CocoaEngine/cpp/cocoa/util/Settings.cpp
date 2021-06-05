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
			extern CPath General::s_ImGuiConfigPath = CPath::create("default.ini");
			extern CPath General::s_EngineAssetsPath = CPath::create("assets");
			extern CPath General::s_EngineExeDirectory = CPath::create();
			extern CPath General::s_EngineSourceDirectory = CPath::create();
			extern CPath General::s_CurrentScene = CPath::create("New Scene.cocoa");
			extern CPath General::s_CurrentProject = CPath::create("New Project.cprj");
			extern CPath General::s_StylesDirectory = CPath::create("assets\\styles");
			extern CPath General::s_WorkingDirectory = CPath::create();
			extern CPath General::s_EditorSaveData = CPath::create("EditorSaveData.json");
			extern CPath General::s_EditorStyleData = CPath::create("EditorStyle.json");
		}

		namespace Physics2D
		{
			// =======================================================================
			// Physics Settings
			// =======================================================================
			extern int Physics2D::s_PositionIterations = 3;
			extern int Physics2D::s_VelocityIterations = 8;
			extern float Physics2D::s_Timestep = 1.0f / 60.0f;
		}
	}
}