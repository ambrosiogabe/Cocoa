#pragma once

#include "externalLibs.h"
#include "jade/core/Core.h"

#include "jade/file/IFile.h"
#include "jade/util/Log.h"

#include "nativeScripting/ScriptParser.h"

#include <filesystem>
#include <iostream>

namespace Jade
{
	namespace CodeGenerators
	{
		static JPath visitedClassBuffer[100];
		static int numVisited = 0;

		static bool visitedSourceFile(UClass clazz)
		{
			for (int i = 0; i < numVisited; i++)
			{
				if (clazz.m_FullFilepath == visitedClassBuffer[i])
				{
					return true;
				}
			}

			return false;
		}

		void GenerateInitFile(const std::vector<UClass>& classes, const JPath& filepath)
		{
			std::ostringstream source;

			source << "#pragma once\n";
			source << "#include \"jade/core/Core.h\"\n";
			source << "#include \"jade/util/Log.h\"\n";
			source << "#include \"jade/core/Entity.h\"\n";

			const std::filesystem::path base = filepath.GetDirectory(-1);
			for (auto clazz : classes)
			{
				const std::filesystem::path otherPath = clazz.m_FullFilepath.Filepath();
				source << "#include \"" << std::filesystem::relative(otherPath, base).generic_string().c_str() << "\"\n";

				std::string genFilename = clazz.m_FullFilepath.GetFilenameWithoutExt() + "-generated" + clazz.m_FullFilepath.FileExt();
				const std::filesystem::path otherGenPath = (JPath(clazz.m_FullFilepath.GetDirectory(-1)) + JPath("generated") + genFilename).Filepath();
				source << "#include \"" << std::filesystem::relative(otherGenPath, base).generic_string().c_str() << "\"\n";
			}

			source << "\n";
			source << "#define ENTT_STANDARD_CPP\n";
			source << "#include <entt/entt.hpp>\n";
			source << "#include <nlohmann/json.hpp>\n\n";

			source << "extern \"C\" namespace Jade\n";
			source << "{\n";
			source << "\textern \"C\" namespace Init\n";
			source << "\t{\n";
			source << "\t\tentt::registry registry = entt::registry();\n\n";

			// AddComponent function
			source << "\t\textern \"C\" JADE_SCRIPT void AddComponent(std::string className, entt::entity entity)\n";
			source << "\t\t{\n";
			source << "\t\t\tif (!registry.valid(entity))\n";
			source << "\t\t\t{\n";
			source << "\t\t\t\tregistry.create(entity);\n";
			source << "\t\t\t}\n";
			source << "\t\t\n\n";

			numVisited = 0;
			for (auto clazz : classes)
			{
				if (!visitedSourceFile(clazz)) 
				{
					std::string namespaceName = "Reflect" + clazz.m_FullFilepath.GetFilenameWithoutExt();
					source << "\t\t\tfor (auto strClass : " << namespaceName.c_str() << "::stringToMap)\n";
					source << "\t\t\t{\n";
					source << "\t\t\t\tif (strClass.first == className)\n";
					source << "\t\t\t\t{\n";
					source << "\t\t\t\t\t" << namespaceName << "::AddComponent(className, entity, registry);\n";
					source << "\t\t\t\t\treturn;\n";
					source << "\t\t\t\t}\n";
					source << "\t\t\t}\n";

					visitedClassBuffer[numVisited] = clazz.m_FullFilepath;
					numVisited++;
				}
			}

			source << "\t\t}\n\n";

			// Generate UpdateScripts function
			source << "\t\textern \"C\" JADE_SCRIPT void UpdateScripts(float dt, Scene* scene)\n";
			source << "\t\t{\n";
			for (auto clazz : classes)
			{
				source << "\t\t\t{\n";
				source << "\t\t\t\tauto view = registry.view<" << clazz.m_ClassName.c_str() << ">();\n";
				source << "\t\t\t\tfor (auto entity : view)\n";
				source << "\t\t\t\t{\n";
				source << "\t\t\t\t\tauto comp = registry.get<" << clazz.m_ClassName.c_str() << ">(entity);\n";
				source << "\t\t\t\t\tcomp.Update(dt);\n";
				source << "\t\t\t\t}\n";
				source << "\t\t\t}\n";
			}
			source << "\t\t}\n";

			// Generate EditorUpdateScripts function
			source << "\n";
			source << "\t\textern \"C\" JADE_SCRIPT void EditorUpdateScripts(float dt, Scene* scene)\n";
			source << "\t\t{\n";
			for (auto clazz : classes)
			{
				source << "\t\t\t{\n";
				source << "\t\t\t\tauto view = registry.view<" << clazz.m_ClassName.c_str() << ">();\n";
				source << "\t\t\t\tfor (auto entity : view)\n";
				source << "\t\t\t\t{\n";
				source << "\t\t\t\t\tauto comp = registry.get<" << clazz.m_ClassName.c_str() << ">(entity);\n";
				source << "\t\t\t\t\tcomp.EditorUpdate(dt);\n";
				source << "\t\t\t\t}\n";
				source << "\t\t\t}\n";
			}
			source << "\t\t}\n";

			// Generate SaveScript function
			source << "\n";
			source << "\t\textern \"C\" JADE_SCRIPT void SaveScripts(json& j)\n";
			source << "\t\t{\n";
			source << "\t\t\tLog::Info(\"Saving scripts\");\n";
			numVisited = 0;
			for (auto clazz : classes)
			{
				if (!visitedSourceFile(clazz))
				{
					std::string namespaceName = "Reflect" + clazz.m_FullFilepath.GetFilenameWithoutExt();
					source << "\t\t\t" << namespaceName.c_str() << "::SaveScripts(j, registry);\n";

					visitedClassBuffer[numVisited] = clazz.m_FullFilepath;
					numVisited++;
				}
			}
			source << "\t\t}\n";

			// Generate Load Scripts function
			source << "\n";
			source << "\t\textern \"C\" JADE_SCRIPT void LoadScript(json& j, Entity entity)\n";
			source << "\t\t{\n";
			numVisited = 0;
			for (auto clazz : classes)
			{
				if (!visitedSourceFile(clazz))
				{
					std::string namespaceName = "Reflect" + clazz.m_FullFilepath.GetFilenameWithoutExt();
					source << "\t\t\t" << namespaceName.c_str() << "::TryLoad(j, entity, registry);\n";

					visitedClassBuffer[numVisited] = clazz.m_FullFilepath;
					numVisited++;
				}
			}
			source << "\t\t}\n";

			// Generate Init Scripts function
			source << "\n";
			source << "\t\textern \"C\" JADE_SCRIPT void InitScripts()\n";
			source << "\t\t{\n";
			source << "\t\t\tLog::Info(\"Initializing scripts\");\n";

			numVisited = 0;
			for (auto clazz : classes)
			{
				if (!visitedSourceFile(clazz))
				{
					std::string namespaceName = "Reflect" + clazz.m_FullFilepath.GetFilenameWithoutExt();
					source << "\t\t\t" << namespaceName.c_str() << "::Init();\n";

					visitedClassBuffer[numVisited] = clazz.m_FullFilepath;
					numVisited++;
				}
			}
			source << "\t\t}\n";

			// Generate Init ImGui function
			source << "\n";
			source << "\t\textern \"C\" JADE_SCRIPT void InitImGui(ImGuiContext* ctx)\n";
			source << "\t\t{\n";
			source << "\t\t\tLog::Info(\"Initializing ImGui\");\n";
			source << "\t\t\tImGui::SetCurrentContext(ctx);\n";
			source << "\t\t}\n";

			// Generate ImGui function
			source << "\n";
			source << "\t\textern \"C\" JADE_SCRIPT void ImGui(Entity entity)\n";
			source << "\t\t{\n";

			numVisited = 0;
			for (auto clazz : classes)
			{
				if (!visitedSourceFile(clazz))
				{
					std::string namespaceName = "Reflect" + clazz.m_FullFilepath.GetFilenameWithoutExt();
					source << "\t\t\t" << namespaceName.c_str() << "::ImGui(entity, registry);\n";

					visitedClassBuffer[numVisited] = clazz.m_FullFilepath;
					numVisited++;
				}
			}
			source << "\t\t}\n";

			// Generate Delete Scripts function
			source << "\n";
			source << "\t\textern \"C\" JADE_SCRIPT void DeleteScripts()\n";
			source << "\t\t{\n";
			source << "\t\t\tLog::Info(\"Deleting Scripts\");\n";

			numVisited = 0;
			for (auto clazz : classes)
			{
				if (!visitedSourceFile(clazz))
				{
					std::string namespaceName = "Reflect" + clazz.m_FullFilepath.GetFilenameWithoutExt();
					source << "\t\t\t" << namespaceName.c_str() << "::DeleteScripts(registry);\n";

					visitedClassBuffer[numVisited] = clazz.m_FullFilepath;
					numVisited++;
				}
			}
			source << "\t\t}\n";

			source << "\t}\n";
			source << "}\n";

			IFile::WriteFile(source.str().c_str(), filepath);
		}

		void GeneratePremakeFile(const JPath& filepath)
		{
			std::string source = ""
				"workspace \"ScriptingWorkspace\"\n"
				"   architecture \"x64\"\n"
				"   \n"
				"   configurations {\n"
				"      \"Debug\",\n"
				"      \"Release\",\n"
				"      \"Dist\"\n"
				"   }\n"
				"   \n"
				"   startproject \"ScriptModule\"\n"
				"   \n"
				"   project \"ScriptModule\"\n"
				"   kind \"SharedLib\"\n"
				"   language \"C++\"\n"
				"   cppdialect \"C++17\"\n"
				"   staticruntime \"off\"\n"
				"   \n"
				"   targetdir(\"bin/%{prj.name}\")\n"
				"   objdir(\"bin-int/%{prj.name}\")\n"
				"   \n"
				"   files {\n"
				"       \"**.h\",\n"
				"       \"**.cpp\",\n"
				"       \"**.hpp\",\n"
				"		\"C:/dev/C++/JadeEngine/JadeEditor/cpp/gui/**.cpp\",\n"
				"		\"C:/dev/C++/JadeEngine/JadeEditor/include/gui/**.h\"\n"
				"   }\n"
				"   \n"
				"   disablewarnings {\n"
				"       \"4251\"\n"
				"   }\n"
				"   \n"
				"   includedirs {\n"
				"       \"%{prj.name}/scripts\",\n"
				"       \"C:/dev/C++/JadeEngine/JadeEngine/include\",\n"
				"       \"C:/dev/C++/JadeEngine/JadeEngine/vendor\",\n"
				"       \"C:/dev/C++/JadeEngine/JadeEngine/vendor/glmVendor\",\n"
				"       \"C:/dev/C++/JadeEngine/JadeEngine/vendor/enttVendor/single_include\",\n"
				"       \"C:/dev/C++/JadeEngine/JadeEngine/vendor/glad/include\",\n"
				"       \"C:/dev/C++/JadeEngine/JadeEngine/vendor/imguiVendor\",\n"
				"       \"C:/dev/C++/JadeEngine/JadeEngine/vendor/box2DVendor/include\",\n"
				"       \"C:/dev/C++/JadeEngine/JadeEngine/vendor/nlohmann-json/single_include\",\n"
				"       \"C:/dev/C++/JadeEngine/JadeEngine/vendor/GLFW/include\",\n"
				"		\"C:/dev/C++/JadeEngine/JadeEditor/include/gui\"\n"
				"   }\n"
				"\n"
				"	symbolspath '$(OutDir)$(TargetName)-$([System.DateTime]::Now.ToString(\"HH_mm_ss_fff\")).pdb'\n"
				"\n"
				"   links {\n"
				"       \"C:/dev/C++/JadeEngine/bin/Debug-windows-x86_64/JadeEngine/JadeEngine.lib\",\n"
				"       \"C:/dev/C++/JadeEngine/bin/Debug-windows-x86_64/JadeEditor/ImGui.lib\"\n"
				"   }\n"
				"   \n"
				"   filter \"system:windows\"\n"
				"   \n"
				"   defines {\n"
				"       \"_JADE_SCRIPT_DLL\"\n"
				"   }\n"
				"   \n"
				"\n"
				"	buildcommands {\n"
				"		\"del /S *.pdb\"\n"
				"	}\n"
				"\n"
				"   postbuildcommands {\n"
				"       \"copy /y \\\"$(SolutionDir)bin\\\\ScriptModule\\\\ScriptModule.dll\\\" \\\"C:\\\\dev\\\\C++\\\\JadeEngine\\\\bin\\\\Debug-windows-x86_64\\\\JadeEditor\\\\ScriptModuleTmp.dll\\\"\"\n"
				"   }\n"
				"\n"
				"   filter \"configurations:Debug\"\n"
				"   runtime \"Debug\"\n"
				"   symbols \"on\"\n"
				"   \n"
				"   \n"
				"   filter \"configurations:Release\"\n"
				"   runtime \"Release\"\n"
				"   optimize \"on\"\n"
				"   \n"
				"   \n"
				"   filter \"configurations:Dist\"\n"
				"   runtime \"Release\"\n"
				"   optimize \"on\"\n";

			IFile::WriteFile(source.c_str(), filepath);
		}
	};
}