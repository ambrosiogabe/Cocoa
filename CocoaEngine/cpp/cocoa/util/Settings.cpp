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
			extern CPath General::s_ImGuiConfigPath = CPath::Create("default.ini");
			extern CPath General::s_EngineAssetsPath = CPath::Create("assets");
			extern CPath General::s_EngineExeDirectory = CPath::Create();
			extern CPath General::s_EngineSourceDirectory = CPath::Create();
			extern CPath General::s_CurrentScene = CPath::Create("New Scene.cocoa");
			extern CPath General::s_CurrentProject = CPath::Create("New Project.cprj");
			extern CPath General::s_StylesDirectory = CPath::Create("assets\\styles");
			extern CPath General::s_WorkingDirectory = CPath::Create();
			extern CPath General::s_EditorSaveData = CPath::Create("EditorSaveData.json");
			extern CPath General::s_EditorStyleData = CPath::Create("EditorStyle.json");
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