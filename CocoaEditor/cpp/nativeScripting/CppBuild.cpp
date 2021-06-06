#include "nativeScripting/CppBuild.h"

#include "cocoa/file/File.h"

namespace Cocoa
{
	namespace CppBuild
	{
		// Internal variables
		static bool buildingCode = false;
		static bool compiling = false;

		void build(const Path& projectDirectory)
		{
			if (!buildingCode && !compiling)
			{
				buildingCode = true;
				const Path pathToBuildScript = PathBuilder(projectDirectory.getDirectory(-1).c_str())
					.join("build.bat")
					.createTmpPath();
				std::string cmdArgs = "vs2019";
				Logger::Info("CMD Args: %s", cmdArgs.c_str());
				File::runProgram(pathToBuildScript, cmdArgs);
				buildingCode = false;
			}
		}

		void compile(const Path& projectDirectory)
		{
			if (!buildingCode && !compiling)
			{
				compiling = true;
				Path pathToBuildScript = PathBuilder(projectDirectory.getDirectory(-1).c_str())
					.join("build.bat")
					.createTmpPath();
				std::string cmdArgs = "compile";
				Logger::Info("CMD Args: %s", cmdArgs.c_str());
				File::runProgram(pathToBuildScript, cmdArgs);
				compiling = false;
			}
		}
	}
}