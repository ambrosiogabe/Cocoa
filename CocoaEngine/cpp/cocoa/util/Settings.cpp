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
			extern CPath General::s_ImGuiConfigPath = NCPath::CreatePath("default.ini");
			extern CPath General::s_EngineAssetsPath = NCPath::CreatePath("assets");
			extern CPath General::s_EngineExeDirectory = NCPath::CreatePath();
			extern CPath General::s_EngineSourceDirectory = NCPath::CreatePath();
			extern CPath General::s_CurrentScene = NCPath::CreatePath("New Scene.cocoa");
			extern CPath General::s_CurrentProject = NCPath::CreatePath("New Project.cprj");
			extern CPath General::s_StylesDirectory = NCPath::CreatePath("assets\\styles");
			extern CPath General::s_WorkingDirectory = NCPath::CreatePath();
			extern CPath General::s_EditorSaveData = NCPath::CreatePath("EditorSaveData.json");
			extern CPath General::s_EditorStyleData = NCPath::CreatePath("EditorStyle.json");
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