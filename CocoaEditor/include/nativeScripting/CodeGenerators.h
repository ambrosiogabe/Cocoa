#ifndef COCOA_EDITOR_CODE_GENERATORS_H
#define COCOA_EDITOR_CODE_GENERATORS_H
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "nativeScripting/ScriptParser.h"

#include "cocoa/file/File.h"

#include <filesystem>

namespace Cocoa
{
	namespace CodeGenerators
	{
		void generateInitFile(const std::vector<UClass>& classes, const Path& filepath);

		void generatePremakeFile(const Path& filepath);

		void generateBuildFile(const Path& filepath, const Path& premakeFilepath);
	}
}

#endif
