project "CocoaEditor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"	
    staticruntime "off"

    fullOutputDir = "../bin/" .. outputdir .. "/%{prj.name}"
    targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
    objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "include/**.h",
        "cpp/**.cpp",
        "tests/**.h",
        "tests/**.cpp",
    }

    disablewarnings { 
        "4251",
        "4131",
        "4267"
    }

    libdirs {
        fullOutputDir
    }

    includedirs {
        "../CocoaEngine",
        "../CocoaEngine/include",
        "../CocoaEngine/vendor",
        "../%{prj.name}/include",
        "../%{prj.name}/tests",
        "../%{IncludeDir.glm}",
        "../%{IncludeDir.entt}",
        "../%{IncludeDir.Glad}",
        "../%{IncludeDir.ImGui}",
        "../%{IncludeDir.Box2D}",
        "../%{IncludeDir.Json}",
        "../%{IncludeDir.GLFW}",
        "../%{IncludeDir.Freetype}",
        "../%{IncludeDir.Libclang}",
		"../%{IncludeDir.CppUtils}"
    }

    links {
        "CocoaEngine",
        "ImGui",
		"GLFW",
        "opengl32.lib",
        "Glad",
        "Box2D",
        "Freetype"
    }

    defines {
        "_CRT_SECURE_NO_WARNINGS",
        "_GABE_CPP_UTILS_IMPORT_DLL"
    }

    filter { "system:windows", "configurations:Debug" }
        buildoptions "/MDd"       
        defines { "COCOA_TEST" }

    filter { "system:windows", "configurations:Release" }
        buildoptions "/MD"

    filter "system:windows"
        systemversion "latest"		

        postbuildcommands {
            "xcopy /s /e /q /y /i \"$(SolutionDir)\\%{prj.name}\\assets\" \"$(OutDir)\\assets\" > nul",
            "copy /y \"$(SolutionDir)\\vendor\\premake\\premake5.exe\" \"$(OutDir)\\premake5.exe\"",
            "copy /y \"$(SolutionDir)\\CocoaEngine\\vendor\\imguiVendor\\bin\\%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}\\ImGui\\ImGui.dll\" \"$(OutDir)\\ImGui.dll\"",
            "copy /y \"$(SolutionDir)\\CocoaEngine\\vendor\\imguiVendor\\bin\\%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}\\ImGui\\ImGui.lib\" \"$(OutDir)\\ImGui.lib\"",
            "copy /y \"$(SolutionDir)\\CocoaEngine\\vendor\\imguiVendor\\bin\\%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}\\ImGui\\ImGui.pdb\" \"$(OutDir)\\ImGui.pdb\"",
            "rmdir /s /q \"$(SolutionDir)\\x64"
        }

        defines {
            "_COCOA_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines {
			"_COCOA_DEBUG",
			"_COCOA_ENABLE_ASSERTS"
		}
        runtime "Debug"
        symbols "on"


    filter "configurations:Release"
        defines "_COCOA_RELEASE"
        runtime "Release"
        optimize "on"


    filter "configurations:Dist"
        defines "_COCOA_DIST"
        runtime "Release"
        optimize "on"