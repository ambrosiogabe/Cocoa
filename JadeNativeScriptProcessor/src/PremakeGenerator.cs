using System;
using System.IO;

namespace JadeNativeScriptProcessor.src
{
	public class PremakeGenerator
	{
		public static void GeneratePremakeFile(string path)
		{
            string source = @"
workspace ""ScriptingWorkspace""
    architecture ""x64""

    configurations {
        ""Debug"", 
        ""Release"",
        ""Dist""
    }

    startproject ""ScriptModule""

project ""ScriptModule""
    kind ""SharedLib""
    language ""C++""
    cppdialect ""C++17""
    staticruntime ""off""

    targetdir (""bin/%{prj.name}"")
    objdir (""bin-int/%{prj.name}"")

    files {
        ""**.h"",
        ""**.cpp"",
        ""**.hpp""
    }

    disablewarnings { 
        ""4251"" 
    }

    includedirs {
        ""%{prj.name}/scripts"",
        ""C:/dev/C++/JadeEngine/JadeEngine/include"",
        ""C:/dev/C++/JadeEngine/JadeEngine/vendor"",
        ""C:/dev/C++/JadeEngine/JadeEngine/vendor/glmVendor"",
        ""C:/dev/C++/JadeEngine/JadeEngine/vendor/enttVendor/single_include"",
        ""C:/dev/C++/JadeEngine/JadeEngine/vendor/glad/include"",
        ""C:/dev/C++/JadeEngine/JadeEngine/vendor/imguiVendor"",
        ""C:/dev/C++/JadeEngine/JadeEngine/vendor/box2DVendor/include"",
        ""C:/dev/C++/JadeEngine/JadeEngine/vendor/nlohmann-json/single_include"",
        ""C:/dev/C++/JadeEngine/JadeEngine/vendor/GLFW/include"",
    }

    links {
        ""C:/dev/C++/JadeEngine/bin/Debug-windows-x86_64/JadeEngine/JadeEngine.lib""
    }

    filter ""system:windows""

        defines {
            ""_JADE_SCRIPT_DLL""
        }

        postbuildcommands {
            ""copy \""$(SolutionDir)\\bin\\ScriptModule\\ScriptModule.dll\"" \""C:\\dev\\C++\\JadeEngine\\bin\\Debug-windows-x86_64\\JadeEditor\\ScriptModule.dll\""""
        }

        filter ""configurations:Debug""
        runtime ""Debug""
        symbols ""on""


    filter ""configurations:Release""
        runtime ""Release""
        optimize ""on""


    filter ""configurations:Dist""
        runtime ""Release""
        optimize ""on""
".Replace("\r\n", "\n");

            File.WriteAllText(path, source);
		}
	}
}
