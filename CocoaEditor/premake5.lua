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
        "cpp/**.cpp"
    }

    disablewarnings { 
        "4251" 
    }

    includedirs {
        "../CocoaEngine",
        "../CocoaEngine/include",
        "../CocoaEngine/vendor",
        "../%{prj.name}/include",
        "../%{IncludeDir.glm}",
        "../%{IncludeDir.entt}",
        "../%{IncludeDir.Glad}",
        "../%{IncludeDir.ImGui}",
        "../%{IncludeDir.Box2D}",
        "../%{IncludeDir.Json}",
        "../%{IncludeDir.GLFW}",
        "../%{IncludeDir.Freetype}",
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
        "ENTT_API_IMPORT"
    }

    filter { "system:windows", "configurations:Debug" }
        buildoptions "/MDd"        

    filter { "system:windows", "configurations:Release" }
        buildoptions "/MD"

    filter "system:windows"
        systemversion "latest"		

        postbuildcommands {
            "xcopy /s /e /q /y /i \"$(SolutionDir)\\%{prj.name}\\assets\" \"$(OutDir)\\assets\" > nul",
            "copy /y \"$(SolutionDir)\\vendor\\premake\\premake5.exe\" \"$(OutDir)\\premake5.exe\"",
            "copy /y \"$(SolutionDir)\\CocoaEngine\\vendor\\imguiVendor\\bin\\%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}\\ImGui\\ImGui.dll\" \"$(OutDir)\\ImGui.dll\"",
            "copy /y \"$(SolutionDir)\\CocoaEngine\\vendor\\imguiVendor\\bin\\%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}\\ImGui\\ImGui.lib\" \"$(OutDir)\\ImGui.lib\"",
            "copy /y \"$(SolutionDir)\\CocoaEngine\\vendor\\imguiVendor\\bin\\%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}\\ImGui\\ImGui.pdb\" \"$(OutDir)\\ImGui.pdb\""
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