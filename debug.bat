@echo off

REM If the cl command is not working we need to set up the path by running vcvarsall.bat
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

devenv
