project "CocoaEngine"
    kind "SharedLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
    objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

	files {
        "Cocoa.h",
		"cpp/**.cpp",
		"include/**.h",
		"../%{prj.name}/vendor/glmVendor/glm/**.hpp",
		"../%{prj.name}/vendor/glmVendor/glm/**.inl",
        "../%{prj.name}/vendor/nlohmann-json/single_include/**.hpp"
	}

    disablewarnings { 
        "4251" 
    }

	defines {
        "_CRT_SECURE_NO_WARNINGS",
        "NOMINMAX",
        "_COCOA_DLL",
        "GLFW_DLL",
		"ENTT_API_EXPORT"
	}

	includedirs {
        "../%{prj.name}/include",
        "../%{prj.name}/vendor",
		"../%{IncludeDir.GLFW}",
		"../%{IncludeDir.Glad}",
        "../%{IncludeDir.Box2D}",
		"../%{IncludeDir.glm}",
        "../%{IncludeDir.stb}",
        "../%{IncludeDir.entt}",
        "../%{IncludeDir.Json}",
	}

	links {
		"GLFW",
        "opengl32.lib",
        "Glad",
        "Box2D"
	}

    filter { "system:windows", "configurations:Debug" }
        buildoptions "/MDd"        

    filter { "system:windows", "configurations:Release" }
        buildoptions "/MD"

	filter "system:windows"
		systemversion "latest"

		defines {
			"GLFW_INCLUDE_NONE",
            "JSON_MultipleHeaders"
		}

		postbuildcommands {
			"if not exist \"$(OutDir)..\\CocoaEditor\" mkdir \"$(OutDir)..\\CocoaEditor\"",
			"copy /y \"$(OutDir)CocoaEngine.dll\" \"$(OutDir)..\\CocoaEditor\\CocoaEngine.dll\"",
			"copy /y \"$(SolutionDir)CocoaEngine\\vendor\\GLFW\\bin\\%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}\\GLFW\\GLFW.dll\" \"$(OutDir)..\\CocoaEditor\\GLFW.dll\""
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
