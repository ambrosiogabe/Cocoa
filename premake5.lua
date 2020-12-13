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
IncludeDir["ImGui"] = "JadeEngine/vendor/imguiVendor"
IncludeDir["glm"] = "JadeEngine/vendor/glmVendor"
IncludeDir["stb"] = "JadeEngine/vendor/stb"
IncludeDir["entt"] = "JadeEngine/vendor/enttVendor/single_include"
IncludeDir["Box2D"] = "JadeEngine/vendor/box2DVendor/include"
IncludeDir["Json"] = "JadeEngine/vendor/nlohmann-json/single_include"

include "JadeEngine"
include "JadeEditor"

include "JadeEngine/vendor/GLFW"
include "JadeEngine/vendor/glad"
include "JadeEngine/vendor/imguiVendor"
include "JadeEngine/vendor/box2DVendor"
