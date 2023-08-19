#pragma once
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "cocoa/file/File.h"
#include "cocoa/util/Settings.h"

#include "nativeScripting/ScriptParser.h"

#include <filesystem>

namespace Cocoa
{
	namespace CodeGenerators
	{
		static std::filesystem::path visitedClassBuffer[100];
		static int numVisited = 0;

		static bool visitedSourceFile(UClass clazz)
		{
			for (int i = 0; i < numVisited; i++)
			{
				if (clazz.fullFilepath == visitedClassBuffer[i])
				{
					return true;
				}
			}

			return false;
		}

		void generateInitFile(const std::vector<UClass>& classes, const std::filesystem::path& filepath)
		{
			std::ostringstream source;

			source << "#pragma once\n";
			source << "#include \"cocoa/core/Core.h\"\n";
			source << "#include \"cocoa/util/Log.h\"\n";
			source << "#include \"cocoa/core/Entity.h\"\n";
			source << "#include \"cocoa/core/EntityStruct.h\"\n";

			const std::filesystem::path base = filepath.parent_path();
			for (auto clazz : classes)
			{
				const std::filesystem::path otherPath = clazz.fullFilepath;
				source << "#include \"" << std::filesystem::relative(otherPath, base).generic_string().c_str() << "\"\n";

				std::string genFilename = clazz.fullFilepath.filename().string() + "-generated" + clazz.fullFilepath.extension().string();
				const std::filesystem::path otherGenCPath = clazz.fullFilepath.parent_path() / "generated" / genFilename;
				const std::filesystem::path otherGenPath = otherGenCPath.parent_path();
				source << "#include \"" << std::filesystem::relative(otherGenPath, base).generic_string().c_str() << "\"\n\n";
			}

			source << "#include \"cocoa/components/TransformStruct.h\"\n";
			source << "#include \"cocoa/components/Tag.h\"\n";
			source << "#include \"cocoa/components/SpriteRenderer.h\"\n";
			source << "#include \"cocoa/components/FontRenderer.h\"\n";
			source << "#include \"cocoa/renderer/CameraStruct.h\"\n";
			source << "#include \"cocoa/components/NoSerialize.h\"\n";
			source << "#include \"cocoa/physics2d/PhysicsComponents.h\"\n";

			source << "\n";
			source << "#define ENTT_STANDARD_CPP\n";
			source << "#include <entt/entt.hpp>\n";
			source << "#include <nlohmann/json.hpp>\n\n";

			source << "extern \"C\" namespace Cocoa\n";
			source << "{\n";
			source << "\textern \"C\" namespace Init\n";
			source << "\t{\n";

			// Init Component Id's -------------------------------------------------------------------------------------------
			source << "\t\tstatic void InitComponentIds(SceneData & scene)\n";
			source << "\t\t{\n";
			source << "\t\t\tNEntity::RegisterComponentType<TransformData>();\n";
			source << "\t\t\tNEntity::RegisterComponentType<Tag>();\n";
			source << "\t\t\tNEntity::RegisterComponentType<SpriteRenderer>();\n";
			source << "\t\t\tNEntity::RegisterComponentType<FontRenderer>();\n";
			source << "\t\t\tNEntity::RegisterComponentType<Rigidbody2D>();\n";
			source << "\t\t\tNEntity::RegisterComponentType<Box2D>();\n";
			source << "\t\t\tNEntity::RegisterComponentType<Circle>();\n";
			source << "\t\t\tNEntity::RegisterComponentType<AABB>();\n";
			source << "\t\t\tNEntity::RegisterComponentType<Camera>();\n";
			source << "\t\t\tNEntity::RegisterComponentType<NoSerialize>();\n";

			for (auto clazz : classes)
			{
				source << "\t\t\tNEntity::RegisterComponentType<" << clazz.className.c_str() << ">();\n";
			}

			source << "\t\t}\n";

			// AddComponent function
			source << "\t\textern \"C\" COCOA_SCRIPT void AddComponent(entt::registry& registryRef, std::string className, entt::entity entity)\n";
			source << "\t\t{\n";
			source << "\t\t\tLog::Assert(registryRef.valid(entity), \"Invalid entity in script\");\n";

			numVisited = 0;
			for (auto clazz : classes)
			{
				if (!visitedSourceFile(clazz))
				{
					std::string namespaceName = "Reflect" + ScriptParser::getFilenameAsClassName(clazz.fullFilepath.filename().string());
					source << "\t\t\tfor (auto strClass : " << namespaceName.c_str() << "::stringToMap)\n";
					source << "\t\t\t{\n";
					source << "\t\t\t\tif (strClass.first == className)\n";
					source << "\t\t\t\t{\n";
					source << "\t\t\t\t\t" << namespaceName << "::AddComponent(className, entity, registryRef);\n";
					source << "\t\t\t\t\treturn;\n";
					source << "\t\t\t\t}\n";
					source << "\t\t\t}\n";

					visitedClassBuffer[numVisited] = clazz.fullFilepath;
					numVisited++;
				}
			}

			source << "\t\t}\n\n";

			// Generate UpdateScripts function
			source << "\t\textern \"C\" COCOA_SCRIPT void UpdateScripts(entt::registry& registryRef, float dt)\n";
			source << "\t\t{\n";
			for (auto clazz : classes)
			{
				source << "\t\t\t{\n";
				source << "\t\t\t\tauto view = registryRef.view<" << clazz.className.c_str() << ">();\n";
				source << "\t\t\t\tfor (auto entity : view)\n";
				source << "\t\t\t\t{\n";
				source << "\t\t\t\t\tauto comp = registryRef.get<" << clazz.className.c_str() << ">(entity);\n";
				source << "\t\t\t\t\tcomp.Update(NEntity::CreateEntity(entity), dt);\n";
				source << "\t\t\t\t}\n";
				source << "\t\t\t}\n";
			}
			source << "\t\t}\n";

			// Generate EditorUpdateScripts function
			source << "\n";
			source << "\t\textern \"C\" COCOA_SCRIPT void EditorUpdateScripts(entt::registry& registryRef, float dt)\n";
			source << "\t\t{\n";
			for (auto clazz : classes)
			{
				source << "\t\t\t{\n";
				source << "\t\t\t\tauto view = registryRef.view<" << clazz.className.c_str() << ">();\n";
				source << "\t\t\t\tfor (auto entity : view)\n";
				source << "\t\t\t\t{\n";
				source << "\t\t\t\t\tauto comp = registryRef.get<" << clazz.className.c_str() << ">(entity);\n";
				source << "\t\t\t\t\tcomp.EditorUpdate(NEntity::CreateEntity(entity), dt);\n";
				source << "\t\t\t\t}\n";
				source << "\t\t\t}\n";
			}
			source << "\t\t}\n";

			// Generate NotifyBeginContact function
			source << "\n";
			source << "\t\textern \"C\" COCOA_SCRIPT void NotifyBeginContact(Entity a, Entity b)\n";
			source << "\t\t{\n";
			for (auto clazz : classes)
			{
				source << "\t\t\t{\n";
				source << "\t\t\t\tif (NEntity::HasComponent<" << clazz.className.c_str() << ">(a))\n";
				source << "\t\t\t\t{\n";
				source << "\t\t\t\t\tNEntity::GetComponent<" << clazz.className.c_str() << ">(a).BeginContact(b);\n";
				source << "\t\t\t\t}\n";
				source << "\t\t\t\tif (NEntity::HasComponent<" << clazz.className.c_str() << ">(b))\n";
				source << "\t\t\t\t{\n";
				source << "\t\t\t\t\tNEntity::GetComponent<" << clazz.className.c_str() << ">(b).BeginContact(a);\n";
				source << "\t\t\t\t}\n";
				source << "\t\t\t}\n";
			}
			source << "\t\t}\n";

			// Generate NotifyEndContact function
			source << "\n";
			source << "\t\textern \"C\" COCOA_SCRIPT void NotifyEndContact(Entity a, Entity b)\n";
			source << "\t\t{\n";
			for (auto clazz : classes)
			{
				source << "\t\t\t{\n";
				source << "\t\t\t\tif (NEntity::HasComponent<" << clazz.className.c_str() << ">(a))\n";
				source << "\t\t\t\t{\n";
				source << "\t\t\t\t\tNEntity::GetComponent<" << clazz.className.c_str() << ">(a).EndContact(b);\n";
				source << "\t\t\t\t}\n";
				source << "\t\t\t\tif (NEntity::HasComponent<" << clazz.className.c_str() << ">(b))\n";
				source << "\t\t\t\t{\n";
				source << "\t\t\t\t\tNEntity::GetComponent<" << clazz.className.c_str() << ">(b).EndContact(a);\n";
				source << "\t\t\t\t}\n";
				source << "\t\t\t}\n";
			}
			source << "\t\t}\n";

			// Generate SaveScript function
			source << "\n";
			source << "\t\textern \"C\" COCOA_SCRIPT void SaveScripts(entt::registry& registryRef, json& j, SceneData* sceneData)\n";
			source << "\t\t{\n";
			source << "\t\t\tLog::Info(\"Saving scripts\");\n";
			numVisited = 0;
			for (auto clazz : classes)
			{
				if (!visitedSourceFile(clazz))
				{
					std::string namespaceName = "Reflect" + ScriptParser::getFilenameAsClassName(clazz.fullFilepath.filename().string());
					source << "\t\t\t" << namespaceName.c_str() << "::SaveScripts(j, registryRef, sceneData);\n";

					visitedClassBuffer[numVisited] = clazz.fullFilepath;
					numVisited++;
				}
			}
			source << "\t\t}\n";

			// Generate Load Scripts function
			source << "\n";
			source << "\t\textern \"C\" COCOA_SCRIPT void LoadScript(entt::registry& registryRef, const json& j, Entity entity)\n";
			source << "\t\t{\n";
			numVisited = 0;
			for (auto clazz : classes)
			{
				if (!visitedSourceFile(clazz))
				{
					std::string namespaceName = "Reflect" + ScriptParser::getFilenameAsClassName(clazz.fullFilepath.filename().string());
					source << "\t\t\t" << namespaceName.c_str() << "::TryLoad(j, entity, registryRef);\n";

					visitedClassBuffer[numVisited] = clazz.fullFilepath;
					numVisited++;
				}
			}
			source << "\t\t}\n";

			// Generate Init Scripts function
			source << "\n";
			source << "\t\textern \"C\" COCOA_SCRIPT void InitScripts(SceneData* sceneData)\n";
			source << "\t\t{\n";
			source << "\t\t\tLog::Info(\"Initializing scripts\");\n";
			source << "\t\t\tInitComponentIds(*sceneData);\n";

			numVisited = 0;
			for (auto clazz : classes)
			{
				if (!visitedSourceFile(clazz))
				{
					std::string namespaceName = "Reflect" + ScriptParser::getFilenameAsClassName(clazz.fullFilepath.filename().string());
					source << "\t\t\t" << namespaceName.c_str() << "::Init();\n";

					visitedClassBuffer[numVisited] = clazz.fullFilepath;
					numVisited++;
				}
			}
			source << "\t\t}\n";

			// Generate Init ImGui function
			source << "\n";
			source << "\t\textern \"C\" COCOA_SCRIPT void InitImGui(void* ctx)\n";
			source << "\t\t{\n";
			source << "\t\t\tLog::Info(\"Initializing ImGui\");\n";
			source << "\t\t\tImGui::SetCurrentContext((ImGuiContext*)ctx);\n";
			source << "\t\t}\n";

			// Generate ImGui function
			source << "\n";
			source << "\t\textern \"C\" COCOA_SCRIPT void ImGui(entt::registry& registryRef, Entity entity)\n";
			source << "\t\t{\n";

			numVisited = 0;
			for (auto clazz : classes)
			{
				if (!visitedSourceFile(clazz))
				{
					std::string namespaceName = "Reflect" + ScriptParser::getFilenameAsClassName(clazz.fullFilepath.filename().string());
					source << "\t\t\t" << namespaceName.c_str() << "::ImGui(entity, registryRef);\n";

					visitedClassBuffer[numVisited] = clazz.fullFilepath;
					numVisited++;
				}
			}
			source << "\t\t}\n";

			// Generate Delete Scripts function
			source << "\n";
			source << "\t\textern \"C\" COCOA_SCRIPT void DeleteScripts()\n";
			source << "\t\t{\n";
			source << "\t\t\tLog::Info(\"Deleting Scripts\");\n";

			numVisited = 0;
			for (auto clazz : classes)
			{
				if (!visitedSourceFile(clazz))
				{
					std::string namespaceName = "Reflect" + ScriptParser::getFilenameAsClassName(clazz.fullFilepath.filename().string());
					source << "\t\t\t" << namespaceName.c_str() << "::DeleteScripts();\n";

					visitedClassBuffer[numVisited] = clazz.fullFilepath;
					numVisited++;
				}
			}
			source << "\t\t}\n";

			source << "\t}\n";
			source << "}\n";

			File::writeFile(source.str().c_str(), filepath);
		}

		void generatePremakeFile(const std::filesystem::path& filepath)
		{
			std::ostringstream stream;
			stream << ""
				"workspace \"ScriptingWorkspace\"\n"
				"	architecture \"x64\"\n"
				"\n"
				"	configurations {\n"
				"		\"Debug\",\n"
				"		\"Release\",\n"
				"		\"Dist\"\n"
				"	}\n"
				"\n"
				"	startproject \"ScriptModule\"\n"
				"\n"
				"	project \"ScriptModule\"\n"
				"	kind \"SharedLib\"\n"
				"	language \"C++\"\n"
				"	cppdialect \"C++17\"\n"
				"	staticruntime \"off\"\n"
				"\n"
				"	targetdir(\"bin/%{prj.name}\")\n"
				"	objdir(\"bin-int/%{prj.name}\")\n"
				"\n"
				"	files {\n"
				"		\"**.h\",\n"
				"		\"**.cpp\",\n"
				"		\"**.hpp\",\n";

			const std::filesystem::path engineSource = Settings::General::engineSourceDirectory;
			stream << "\t\t\"" << engineSource << "/CocoaEditor/cpp/gui/**.cpp\",\n";
			stream << "\t\t\"" << engineSource << "/CocoaEditor/include/gui/**.h\"\n";

			stream << ""
				"	}\n"
				"\n"
				"	disablewarnings {\n"
				"		\"4251\"\n"
				"	}\n"
				"\n"
				"	includedirs {\n"
				"		\"%{prj.name}/scripts\",\n";

			stream << "\t\t\"" << engineSource << "/CocoaEngine/include\",\n";
			stream << "\t\t\"" << engineSource << "/CocoaEngine/vendor\",\n";
			stream << "\t\t\"" << engineSource << "/CocoaEngine/vendor/glmVendor\",\n";
			stream << "\t\t\"" << engineSource << "/CocoaEngine/vendor/enttVendor/src\",\n";
			stream << "\t\t\"" << engineSource << "/CocoaEngine/vendor/glad/include\",\n";
			stream << "\t\t\"" << engineSource << "/CocoaEngine/vendor/imguiVendor\",\n";
			stream << "\t\t\"" << engineSource << "/CocoaEngine/vendor/box2DVendor/include\",\n";
			stream << "\t\t\"" << engineSource << "/CocoaEngine/vendor/nlohmann-json/single_include\",\n";
			stream << "\t\t\"" << engineSource << "/CocoaEngine/vendor/GLFW/include\",\n";
			stream << "\t\t\"" << engineSource << "/CocoaEditor/include/gui\"\n";

			stream << ""
				"	}\n"
				"\n"
				"	symbolspath '$(OutDir)$(TargetName)-$([System.DateTime]::Now.ToString(\"HH_mm_ss_fff\")).pdb'\n"
				"\n"
				"	links {\n";

			const std::filesystem::path engineExeDir = Settings::General::engineExeDirectory;
			const std::filesystem::path engineDllDir = Settings::General::engineExeDirectory.parent_path()/"CocoaEngine";
			stream << "\t\t\"" << engineDllDir << "/CocoaEngine.lib\",\n";
			stream << "\t\t\"" << engineExeDir << "/ImGui.lib\"\n";

			stream << ""
				"	}\n"
				"\n"
				"	filter \"system:windows\"\n"
				"\n"
				"	defines {\n"
				"		\"_COCOA_SCRIPT_DLL\"\n"
				"	}\n"
				"\n"
				"	buildcommands {\n"
				"		\"del /S *.pdb\"\n"
				"	}\n"
				"\n"
				"	postbuildcommands {\n";

			stream << "\t\t\"copy /y \\\"$(SolutionDir)bin\\\\ScriptModule\\\\ScriptModule.dll\\\" \\\"" << engineExeDir << "/ScriptModuleTmp.dll\\\"\"\n";

			stream << ""
				"	}\n"
				"\n"
				"	filter \"configurations:Debug\"\n"
				"		runtime \"Debug\"\n"
				"		symbols \"on\"\n"
				"\n"
				"\n"
				"	filter \"configurations:Release\"\n"
				"		runtime \"Release\"\n"
				"		optimize \"on\"\n"
				"\n"
				"\n"
				"	filter \"configurations:Dist\"\n"
				"		runtime \"Release\"\n"
				"		optimize \"on\"\n";

			File::writeFile(stream.str().c_str(), filepath);
		}

		void generateBuildFile(const std::filesystem::path& filepath, const std::filesystem::path& premakeFilepath)
		{
			std::filesystem::path projectPremakeLua = filepath.parent_path() / "premake5.lua";

			std::ostringstream stream;

			stream << R"(
@echo off

IF "%~1" == "" GOTO PrintHelp
IF "%~1" == "compile" GOTO Compile)";

			stream << "\n\n" << premakeFilepath.string();
			stream << " %1 --file=\"";
			stream << projectPremakeLua.string()  << "\"";

			stream << R"(

GOTO Done

:PrintHelp

echo.
echo Enter 'build.bat action' where action is one of the following :
echo.
echo   compile           Generate appropriate project files, then compile solution.
echo   clean             Remove all binaries and intermediate binaries and project files.
echo   codelite          Generate CodeLite project files
echo   gmake2            Generate GNU makefiles for Linux
echo   vs2005            Generate Visual Studio 2005 project files
echo   vs2008            Generate Visual Studio 2008 project files
echo   vs2010            Generate Visual Studio 2010 project files
echo   vs2012            Generate Visual Studio 2012 project files
echo   vs2013            Generate Visual Studio 2013 project files
echo   vs2015            Generate Visual Studio 2015 project files
echo   vs2017            Generate Visual Studio 2017 project files
echo   vs2019            Generate Visual Studio 2019 project files
echo   xcode4            Generate Apple Xcode 4 project files

GOTO Done

:Compile)";

			stream << "\n\n" << premakeFilepath.string();
			stream << "vs2019 --file=";
			stream << projectPremakeLua.string()  << "\"";

			stream << R"(
if not defined DevEnvDir (
	call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat"
)

set solutionFile=")";
			// TODO: Maybe set scripting workspace to project name?
			std::filesystem::path solutionFile = filepath.parent_path() / "ScriptingWorkspace.sln";
			stream << solutionFile.string() << "\"";
			
			stream << R"(
msbuild /t:Build /p:Configuration=Debug /p:Platform=x64 %solutionFile%

:Done
)";


			File::writeFile(stream.str().c_str(), filepath);
		}
	}
}