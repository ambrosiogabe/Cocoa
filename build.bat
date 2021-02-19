@echo off

Rem Build freetype library
mkdir CocoaEngine\vendor\freetypeVendor\freetype2.compiled
cd CocoaEngine\vendor\freetypeVendor\freetype2.compiled
cmake ..\
cd ..\..\..\..\

.\vendor\premake\premake5.exe vs2019