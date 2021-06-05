#ifndef COCOA_ENGINE_SETTINGS_H
#define COCOA_ENGINE_SETTINGS_H
#include "externalLibs.h"
#include "cocoa/file/CPath.h"

namespace Cocoa
{
	namespace Settings
	{
		namespace General
		{
			extern COCOA CPath imGuiConfigPath;
			extern COCOA CPath engineAssetsPath;
			extern COCOA CPath engineExeDirectory;
			extern COCOA CPath engineSourceDirectory;
			extern COCOA CPath workingDirectory;
			extern COCOA CPath currentProject;
			extern COCOA CPath stylesDirectory;
			extern COCOA CPath currentScene;
			extern COCOA CPath editorSaveData;
			extern COCOA CPath editorStyleData;
			extern COCOA CPath editorStyle;
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
