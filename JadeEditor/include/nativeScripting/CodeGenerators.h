#pragma once

#include "externalLibs.h"
#include "jade/core/Core.h"

#include "jade/file/IFile.h"

#include "nativeScripting/ScriptParser.h"

#include <filesystem>
#include <iostream>

namespace Jade
{
	namespace CodeGenerators
	{
        void GenerateInitFile(const std::list<UClass>& classes, const JPath& filepath)
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
            }

            source << "\n";
            source << "extern \"C\" namespace Jade\n";
            source << "{\n";
            source << "\textern \"C\" namespace Init\n";
            source << "\t{\n";

            // Generate UpdateScripts function
            source << "\t\textern \"C\" JADE_SCRIPT void UpdateScripts(float dt, Scene* scene)\n";
            source << "\t\t{\n";
            for (auto clazz : classes) 
            {
                source << "\t\t\t{\n";
                source << "\t\t\t\tauto view = scene->GetRegistry().view<" << clazz.m_ClassName.c_str() << ">();\n";
                source << "\t\t\t\tfor (auto entity : view)\n";
                source << "\t\t\t\t{\n";
                source << "\t\t\t\t\tEntity jadeEntity = Entity(entity, scene);\n";
                source << "\t\t\t\t\tjadeEntity.GetComponent<" << clazz.m_ClassName.c_str() << ">().Update(dt);\n";
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
                source << "\t\t\t\tauto view = scene->GetRegistry().view<" << clazz.m_ClassName.c_str() << ">();\n";
                source << "\t\t\t\tfor (auto entity : view)\n";
                source << "\t\t\t\t{\n";
                source << "\t\t\t\t\tEntity jadeEntity = Entity(entity, scene);\n";
                source << "\t\t\t\t\tjadeEntity.GetComponent<" << clazz.m_ClassName.c_str() << ">().EditorUpdate(dt);\n";
                source << "\t\t\t\t}\n";
                source << "\t\t\t}\n";
            }
            source << "\t\t}\n";

            // Generate SaveScript function
            source << "\n";
            source << "\t\textern \"C\" JADE_SCRIPT void SaveScript(const Script& script, Entity entity)\n";
            source << "\t\t{\n";
            source << "\t\t\tLog::Info(\"Saving script: \");\n";
            source << "\t\t}\n";

            // Generate Load Scripts function
            source << "\n";
            source << "\t\textern \"C\" JADE_SCRIPT void LoadScripts()\n";
            source << "\t\t{\n";
            source << "\t\t\tLog::Info(\"Loading Scripts\");\n";
            source << "\t\t}\n";

            // Generate Load Scripts function
            source << "\n";
            source << "\t\textern \"C\" JADE_SCRIPT void DeleteScripts()\n";
            source << "\t\t{\n";
            source << "\t\t\tLog::Info(\"Deleting Scripts\");\n";
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
                "       \"**.hpp\"\n"
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
                "   }\n"
                "   \n"
                "   links {\n"
                "       \"C:/dev/C++/JadeEngine/bin/Debug-windows-x86_64/JadeEngine/JadeEngine.lib\"\n"
                "   }\n"
                "   \n"
                "   filter \"system:windows\"\n"
                "   \n"
                "   defines {\n"
                "       \"_JADE_SCRIPT_DLL\"\n"
                "   }\n"
                "   \n"
                "   postbuildcommands {\n"
                "       \"copy \\\"$(SolutionDir)\\\\bin\\\\ScriptModule\\\\ScriptModule.dll\\\\\\\" \\\"C:\\\\dev\\\\C++\\\\JadeEngine\\\\bin\\\\Debug-windows-x86_64\\\\JadeEditor\\\\ScriptModule.dll\\\\\"\n"
                "   }\n"
                "   \n"
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