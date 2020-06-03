@echo off

REM Notes:
set CommonCompilerFlags=-I..\src\vendor -I..\src\include -std:c++17
set CommonLinkerFlags=user32.lib gdi32.lib opengl32.lib

set Files="..\src\cpp\winMain.cpp" "..\src\cpp\jade\platform\windows\*.cpp" "..\src\cpp\jade\scenes\*.cpp"

REM If the cl command is not working we need to set up the path by running vcvarsall.bat
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

mkdir %~dp0build
pushd %~dp0build
REM TODO: Put commands to copy assets folder into here
REM Debug Build
cl %CommonCompilerFlags% -Z7 /EHsc %Files% %CommonLinkerFlags%
popd
