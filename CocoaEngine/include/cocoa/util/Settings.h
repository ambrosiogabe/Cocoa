#ifndef COCOA_ENGINE_SETTINGS_H
#define COCOA_ENGINE_SETTINGS_H
#include "externalLibs.h"

#include <filesystem>

namespace Cocoa
{
	namespace Settings
	{
		namespace General
		{
			extern COCOA std::filesystem::path imGuiConfigPath;
			extern COCOA std::filesystem::path engineAssetsPath;
			extern COCOA std::filesystem::path engineExeDirectory;
			extern COCOA std::filesystem::path engineSourceDirectory;
			extern COCOA std::filesystem::path workingDirectory;
			extern COCOA std::filesystem::path currentProject;
			extern COCOA std::filesystem::path stylesDirectory;
			extern COCOA std::filesystem::path currentScene;
			extern COCOA std::filesystem::path editorSaveData;
			extern COCOA std::filesystem::path editorStyleData;
			extern COCOA std::filesystem::path editorStyle;
		};

		namespace Physics2D
		{
			extern COCOA int velocityIterations;
			extern COCOA int positionIterations;
			extern COCOA float timestep;
		};
	}
}

#endif
