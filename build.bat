@echo off

REM Run python script to collect all cpp files into unity build header file
python .\prebuild.py

REM Include directories and compiler flags
set IncludeDirs=/I..\jade\vendor /I..\jade\vendor\imgui /I..\jade\include 
set EngineCompilerFlags=/I.. /D_JADE_DLL /D_WINDLL /c /Zl /FoJade.obj
set AppCompilerFlags=/I..\jade

REM Common compiler flags
set DebugCommonCompilerFlags= %IncludeDirs% /Zi /MTd /EHsc /std:c++17 /W4 /wd4201 /wd4100 /wd4312 /D_CRT_SECURE_NO_WARNINGS /DNOMINMAX /D_JADE_PLATFORM_WINDOWS
set DebugCommonLinkerFlags=/link /INCREMENTAL /NOLOGO user32.lib gdi32.lib opengl32.lib

REM Main cpp files
set JadeMain="..\jade\cpp\core\Main.cpp"
set AppMain="..\application\cpp\app.cpp"

REM Create directories if needed for binaries and intermediate binaries
if NOT EXIST %~dp0"bin-int" mkdir %~dp0"bin-int"
if NOT EXIST %~dp0"bin" mkdir %~dp0"bin"
if NOT EXIST %~dp0"bin-int\Jade" mkdir %~dp0"bin-int\Jade"
if NOT EXIST %~dp0"bin\Jade" mkdir %~dp0"bin\Jade"
if NOT EXIST %~dp0"bin-int\LevelEditor" mkdir %~dp0"bin-int\LevelEditor"
if NOT EXIST %~dp0"bin\LevelEditor" mkdir %~dp0"bin\LevelEditor"

REM If the cl command is not working we need to set up the path by running vcvarsall.bat
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat" > nul

REM Change into bin-int directory and build everything, then move files and stuff
pushd %~dp0"bin-int"
xcopy /s /e /q /y /i "..\assets" "..\bin\Jade\assets" > nul
xcopy /s /e /q /y /i "..\assets" "..\bin\LevelEditor\assets" > nul

REM Debug Build
cl %DebugCommonCompilerFlags% %EngineCompilerFlags% %JadeMain% 
lib /NOLOGO Jade.obj
move *.* "Jade" > nul
copy "Jade\Jade.lib" "Jade.lib" > nul
copy "Jade\vc140.pdb" "vc140.pdb" > nul
move "Jade\*.lib" "..\bin\Jade" > nul
move "Jade\*.pdb" "..\bin\Jade" > nul

cl %DebugCommonCompilerFlags% %AppCompilerFlags% %AppMain% %DebugCommonLinkerFlags% Jade.lib
move *.* "LevelEditor" > nul
move "LevelEditor\*.ilk" "..\bin\LevelEditor" > nul
move "LevelEditor\*.exe" "..\bin\LevelEditor" > nul
move "LevelEditor\*.pdb" "..\bin\LevelEditor" > nul

REM Release build
REM cl %CommonCompilerFlags% /Ox /EHsc %Files% %CommonLinkerFlags%
popd