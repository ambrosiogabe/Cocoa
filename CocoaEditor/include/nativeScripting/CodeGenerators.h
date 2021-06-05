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
		void generateInitFile(const std::vector<UClass>& classes, const CPath& filepath);

		void generatePremakeFile(const CPath& filepath);

		void generateBuildFile(const CPath& filepath, const CPath& premakeFilepath);
	}
}

#endif
