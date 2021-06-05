#include "nativeScripting/CppBuild.h"

#include "cocoa/file/File.h"

namespace Cocoa
{
	namespace CppBuild
	{
		// Internal variables
		static bool buildingCode = false;
		static bool compiling = false;

		void Build(const CPath& projectDirectory)
		{
			if (!buildingCode && !compiling)
			{
				buildingCode = true;
				CPath pathToBuildScript = CPath::create(projectDirectory.getDirectory(-1));
				pathToBuildScript.join(CPath::create("build.bat"));
				std::string cmdArgs = "vs2019";
				Logger::Info("CMD Args: %s", cmdArgs.c_str());
				File::runProgram(pathToBuildScript, cmdArgs);
				buildingCode = false;
			}
		}

		void Compile(const CPath& projectDirectory)
		{
			if (!buildingCode && !compiling)
			{
				compiling = true;
				CPath pathToBuildScript = CPath::create(projectDirectory.getDirectory(-1));
				pathToBuildScript.join(CPath::create("build.bat"));
				std::string cmdArgs = "compile";
				Logger::Info("CMD Args: %s", cmdArgs.c_str());
				File::runProgram(pathToBuildScript, cmdArgs);
				compiling = false;
			}
		}
	}
}