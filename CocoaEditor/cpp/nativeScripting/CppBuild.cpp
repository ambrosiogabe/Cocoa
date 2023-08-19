#include "nativeScripting/CppBuild.h"

#include "cocoa/file/File.h"

namespace Cocoa
{
	namespace CppBuild
	{
		// Internal variables
		static bool buildingCode = false;
		static bool compiling = false;

		void build(const std::filesystem::path& projectDirectory)
		{
			if (!buildingCode && !compiling)
			{
				buildingCode = true;
				const std::filesystem::path pathToBuildScript = projectDirectory.parent_path() / "build.bat";
				std::string cmdArgs = "vs2019";
				Logger::Info("CMD Args: %s", cmdArgs.c_str());
				File::runProgram(pathToBuildScript, cmdArgs);
				buildingCode = false;
			}
		}

		void compile(const std::filesystem::path& projectDirectory)
		{
			if (!buildingCode && !compiling)
			{
				compiling = true;
				std::filesystem::path pathToBuildScript = projectDirectory.parent_path() / "build.bat";
				std::string cmdArgs = "compile";
				Logger::Info("CMD Args: %s", cmdArgs.c_str());
				File::runProgram(pathToBuildScript, cmdArgs);
				compiling = false;
			}
		}
	}
}