project "JadeNativeScriptProcessor"
    kind "ConsoleApp"
    language "C#"
    dotnetframework "4.6.1"

    editorOutputDir = "/bin/" .. outputdir .. "/JadeEditor"
    fullOutputDir = "/bin/" .. outputdir .. "/%{prj.name}"
    targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
    objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "src/**.cs"
    }

    filter "system:windows"
        postbuildcommands {
            "copy /y \"$(SolutionDir)%{fullOutputDir}\\JadeNativeScriptProcessor.exe\" \"$(SolutionDir)%{editorOutputDir}\\JadeNativeScriptProcessor.exe\"",
            "xcopy /s /e /q /y /i \"$(SolutionDir)%{fullOutputDir}\\*.dll\" \"$(SolutionDir)%{editorOutputDir}\\\" > nul",
        }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"


    filter "configurations:Release"
        runtime "Release"
        optimize "on"


    filter "configurations:Dist"
        runtime "Release"
        optimize "on"