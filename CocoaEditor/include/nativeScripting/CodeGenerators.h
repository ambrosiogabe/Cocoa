#pragma once
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "nativeScripting/ScriptParser.h"

#include "cocoa/file/File.h"
#include "cocoa/util/Log.h"
#include "cocoa/util/Settings.h"

#include <filesystem>

namespace Cocoa
{
	namespace CodeGenerators
	{
		void GenerateInitFile(const std::vector<UClass>& classes, const CPath& filepath);

		void GeneratePremakeFile(const CPath& filepath);

		void GenerateBuildFile(const CPath& filepath, const CPath& premakeFilepath);
	}
}