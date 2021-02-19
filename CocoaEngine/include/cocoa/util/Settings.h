#pragma once
#include "externalLibs.h"
#include "cocoa/file/CPath.h"

namespace Cocoa
{
	namespace Settings
	{
		namespace General
		{
			extern COCOA CPath s_ImGuiConfigPath;
			extern COCOA CPath s_EngineAssetsPath;
			extern COCOA CPath s_EngineExeDirectory;
			extern COCOA CPath s_EngineSourceDirectory;
			extern COCOA CPath s_WorkingDirectory;
			extern COCOA CPath s_CurrentProject;
			extern COCOA CPath s_StylesDirectory;
			extern COCOA CPath s_CurrentScene;
			extern COCOA CPath s_EditorSaveData;
			extern COCOA CPath s_EditorStyleData;
			extern COCOA CPath s_EditorStyle;
		};

		namespace Physics2D
		{
			extern COCOA int s_VelocityIterations;
			extern COCOA int s_PositionIterations;
			extern COCOA float s_Timestep;
		};
	}
}