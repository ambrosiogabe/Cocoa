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
		void generateInitFile(const std::vector<UClass>& classes, const std::filesystem::path& filepath);

		void generatePremakeFile(const std::filesystem::path& filepath);

		void generateBuildFile(const std::filesystem::path& filepath, const std::filesystem::path& premakeFilepath);
	}
}

#endif
