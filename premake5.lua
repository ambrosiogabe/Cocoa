workspace "CocoaEngine"
    architecture "x64"

    configurations { 
        "Debug", 
        "Release",
        "Dist"
    }

    startproject "CocoaEditor"

-- This is a helper variable, to concatenate the sys-arch
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "CocoaEngine/vendor/GLFW/include"
IncludeDir["Glad"] = "CocoaEngine/vendor/glad/include"
IncludeDir["ImGui"] = "CocoaEngine/vendor/imguiVendor"
IncludeDir["glm"] = "CocoaEngine/vendor/glmVendor"
IncludeDir["stb"] = "CocoaEngine/vendor/stb"
IncludeDir["entt"] = "CocoaEngine/vendor/enttVendor/single_include"
IncludeDir["Box2D"] = "CocoaEngine/vendor/box2DVendor/include"
IncludeDir["Json"] = "CocoaEngine/vendor/nlohmann-json/single_include"
IncludeDir["Freetype"] = "CocoaEngine/vendor/freetypeVendor/include"

include "CocoaEngine"
include "CocoaEditor"

include "CocoaEngine/vendor/GLFW"
include "CocoaEngine/vendor/glad"
include "CocoaEngine/vendor/imguiVendor"
include "CocoaEngine/vendor/box2DVendor"

externalproject "Freetype"
    location "CocoaEngine/vendor/freetypeVendor/freetype2.compiled"
    uuid "55FB27B1-C655-3244-9BAA-DDACB0BD93F7"
    kind "SharedLib"
    language "C"
