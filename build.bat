@echo off

if not exist JadeEditor\vendor\roslynVendor\artifacts\ (
	echo "Building Roslyn library. This may take awhile."
	echo "Restoring Roslyn library."
	JadeEditor\vendor\roslynVendor\Restore.cmd
	JadeEditor\vendor\roslynVendor\Build.cmd -Configuration Release -deployExtensions -launch
)

if not exist JadeEditor\vendor\roslynVendor\artifacts\bin\csc\Release\ (
	echo "Building Roslyn library.
	JadeEditor\vendor\roslynVendor\Build.cmd -Configuration Release -deployExtensions -launch
)

.\vendor\premake\premake5.exe vs2019