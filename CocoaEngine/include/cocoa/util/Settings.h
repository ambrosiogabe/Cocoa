#ifndef COCOA_ENGINE_SETTINGS_H
#define COCOA_ENGINE_SETTINGS_H
#include "externalLibs.h"
#include "cocoa/file/Path.h"

namespace Cocoa
{
	namespace Settings
	{
		namespace General
		{
			extern COCOA PathBuilder imGuiConfigPath;
			extern COCOA PathBuilder engineAssetsPath;
			extern COCOA PathBuilder engineExeDirectory;
			extern COCOA PathBuilder engineSourceDirectory;
			extern COCOA PathBuilder workingDirectory;
			extern COCOA PathBuilder currentProject;
			extern COCOA PathBuilder stylesDirectory;
			extern COCOA PathBuilder currentScene;
			extern COCOA PathBuilder editorSaveData;
			extern COCOA PathBuilder editorStyleData;
			extern COCOA PathBuilder editorStyle;
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
