project "JadeEditor"
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
        "../JadeEngine",
        "../JadeEngine/include",
        "../JadeEngine/vendor",
        "../%{prj.name}/include",
        "../%{IncludeDir.glm}",
        "../%{IncludeDir.entt}",
        "../%{IncludeDir.Glad}",
        "../%{IncludeDir.ImGui}",
        "../%{IncludeDir.Box2D}",
        "../%{IncludeDir.Json}",
        "../%{IncludeDir.GLFW}",
        "../%{IncludeDir.Mono}",
    }

    links {
        "JadeEngine",
        "ImGui",
		"GLFW",
        "opengl32.lib",
        "Glad",
        "Box2D",
    }

    filter { "system:windows", "configurations:Debug" }
        buildoptions "/MDd"        

    filter { "system:windows", "configurations:Release" }
        buildoptions "/MD"

    filter "system:windows"
        systemversion "latest"		

        postbuildcommands {
            "xcopy /s /e /q /y /i \"$(SolutionDir)\\%{prj.name}\\assets\" \"$(SolutionDir)\\%{fullOutputDir}\\assets\" > nul",
            "copy \"$(SolutionDir)\\vendor\\premake\\premake5.exe\" \"$(SolutionDir)\\%{fullOutputDir}\\premake5.exe\""
        }

        defines {
            "_JADE_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines {
			"_JADE_DEBUG",
			"_JADE_ENABLE_ASSERTS"
		}
        runtime "Debug"
        symbols "on"


    filter "configurations:Release"
        defines "_JADE_RELEASE"
        runtime "Release"
        optimize "on"


    filter "configurations:Dist"
        defines "_JADE_DIST"
        runtime "Release"
        optimize "on"