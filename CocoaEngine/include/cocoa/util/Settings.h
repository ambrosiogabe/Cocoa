#pragma once
#include "externalLibs.h"
#include "cocoa/file/CPath.h"

namespace Cocoa
{
	namespace Settings
	{
		class COCOA General
		{
		public:
			static CPath s_ImGuiConfigPath;
			static CPath s_EngineAssetsPath;
			static CPath s_EngineExeDirectory;
			static CPath s_EngineSourceDirectory;
			static CPath s_WorkingDirectory;
			static CPath s_CurrentProject;
			static CPath s_StylesDirectory;
			static CPath s_CurrentScene;
			static CPath s_EditorSaveData;
			static CPath s_EditorStyleData;
			static CPath s_EditorStyle;
		};

		class COCOA Physics2D
		{
		public:
			static int s_VelocityIterations;
			static int s_PositionIterations;
			static float s_Timestep;
		};
	}
}