workspace "JadeEngine"
    architecture "x64"

    configurations { 
        "Debug", 
        "Release",
        "Dist"
    }

    startproject "JadeEditor"

-- This is a helper variable, to concatenate the sys-arch
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "JadeEngine/vendor/GLFW/include"
IncludeDir["Glad"] = "JadeEngine/vendor/glad/include"
IncludeDir["ImGui"] = "JadeEngine/vendor/imgui"
IncludeDir["glm"] = "JadeEngine/vendor/glm"
IncludeDir["stb"] = "JadeEngine/vendor/stb"
IncludeDir["entt"] = "JadeEngine/vendor/entt"
IncludeDir["Box2D"] = "JadeEngine/vendor/box2DVendor/include"
IncludeDir["Json"] = "JadeEngine/vendor/nlohmann-json/single_include"

include "JadeEngine/vendor/GLFW"
include "JadeEngine/vendor/glad"
include "JadeEngine/vendor/imgui"
include "JadeEngine/vendor/box2DVendor"
include "JadeEngine/vendor/nlohmann-json"

project "JadeEngine"
    location "JadeEngine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
        "%{prj.name}/Jade.h",
		"%{prj.name}/cpp/**.cpp",
		"%{prj.name}/include/**.h",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl"
	}

	defines {
        "_CRT_SECURE_NO_WARNINGS",
        "NOMINMAX",
	}

	includedirs {
        "%{prj.name}/include",
        "%{prj.name}/vendor",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
        "%{IncludeDir.Box2D}",
		"%{IncludeDir.glm}",
        "%{IncludeDir.stb}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.Json}"
	}

	links {
		"GLFW",
        "opengl32.lib",
        "Glad",
		"ImGui",
        "Box2D",
        "Json"
	}

	filter "system:windows"
		systemversion "latest"

		defines {
			"GLFW_INCLUDE_NONE"
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
        


project "JadeEditor"
    location "JadeEditor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"	
    staticruntime "on"

    fullOutputDir = "bin/" .. outputdir .. "/%{prj.name}"
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "%{prj.name}/include/**.h",
        "%{prj.name}/cpp/**.cpp"
    }

    includedirs {
        "JadeEngine",
        "JadeEngine/include",
        "JadeEngine/vendor",
        "%{prj.name}/include",
        "%{IncludeDir.glm}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.Box2D}",
        "%{IncludeDir.Json}"
    }

    links {
        "JadeEngine"
    }

    filter "system:windows"
        systemversion "latest"
        postbuildcommands {
            "xcopy /s /e /q /y /i \"$(SolutionDir)\\%{prj.name}\\assets\" \"$(SolutionDir)\\%{fullOutputDir}\\assets\" > nul"
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