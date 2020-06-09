@echo off

python .\prebuild.py

REM Notes:
set CommonCompilerFlags=-I..\src\vendor -I..\src\include -I.. -std:c++17
set CommonLinkerFlags=user32.lib gdi32.lib opengl32.lib

REM The comment below was for when I was compiling without my handy dandy python script. Now I do a unity build.
REM set Files="..\src\cpp\winMain.cpp" "..\src\cpp\jade\platform\windows\*.cpp" "..\src\cpp\jade\scenes\*.cpp" "..\src\cpp\jade\renderer\*.cpp"
set Files="..\src\cpp\winMain.cpp"

REM If the cl command is not working we need to set up the path by running vcvarsall.bat
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

mkdir %~dp0build
pushd %~dp0build
REM TODO: Put commands to copy assets folder into here
REM Debug Build
cl %CommonCompilerFlags% -Zi -EHsc %Files% %CommonLinkerFlags%
REM Release build
REM cl %CommonCompilerFlags% -Ox -EHsc %Files% %CommonLinkerFlags%
popd
