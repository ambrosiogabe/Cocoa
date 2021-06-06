#include "nativeScripting/SourceFileWatcher.h"

#include "util/Settings.h"
#include "nativeScripting/ScriptParser.h"
#include "nativeScripting/CodeGenerators.h"
#include "nativeScripting/CppBuild.h"

#include "cocoa/file/File.h"
#include "cocoa/util/Settings.h"

namespace Cocoa
{
	// Internal variables
	static Path rootDir = Path::createDefault();
	static Path projectPremakeLua = Path::createDefault();
	static auto classes = std::vector<UClass>();
	static bool fileModified = false;
	static bool buildingCode = false;

	// Internal functions
	static void GenerateBuildFile();

	SourceFileWatcher::SourceFileWatcher(Path rootDirectory)
		: mRootDirectory(rootDirectory)
	{
		startFileWatcher();
	}

	static bool IsHeaderFile(const Path& file)
	{
		return strcmp(file.fileExt, ".h") == 0 || strcmp(file.fileExt, ".hpp") == 0;
	}

	static bool IsReflectionHeaderFile(const Path& file)
	{
		return strcmp(file.filename, "Reflection") == 0;
	}

	static void AddClassIfNotExist(UClass clazz)
	{
		for (int i = 0; i < classes.size(); i++)
		{
			UClass& c = classes.at(i);
			if (c.className == clazz.className)
			{
				classes[i].className = clazz.className;
				classes[i].fullFilepath = clazz.fullFilepath;
				classes[i].variables = clazz.variables;
				return;
			}
		}

		classes.push_back(clazz);
	}

	static std::vector<UClass>::iterator FindClass(UClass& c, std::vector<UClass>& classesToSearch)
	{
		for (auto classIter = classesToSearch.begin(); classIter != classesToSearch.end(); classIter++)
		{
			if (classIter->className == c.className)
			{
				return classIter;
			}
		}

		return classesToSearch.end();
	}

	static void MergeNewClasses(std::vector<UClass>& classesToMerge, const Path& filepath)
	{
		for (auto clazz : classesToMerge)
		{
			auto c = FindClass(clazz, classes);
			if (c != classes.end())
			{
				c->className = clazz.className;
				c->fullFilepath = clazz.fullFilepath;
				c->variables = clazz.variables;
			}
			else
			{
				classes.push_back(clazz);
			}
		}

		for (auto classIter = classes.begin(); classIter != classes.end(); classIter++)
		{
			if (classIter->fullFilepath == filepath)
			{
				if (FindClass(*classIter, classesToMerge) == classesToMerge.end())
				{
					classIter = classes.erase(classIter);
				}
			}
		}
	}

	static void GenerateInitFiles()
	{
		Path generatedDir = PathBuilder(rootDir)
			.join("generated")
			.createTmpPath();
		File::createDirIfNotExists(generatedDir);

		Path initH = PathBuilder(generatedDir)
			.join("init.h")
			.createTmpPath();
		Path initCpp = PathBuilder(generatedDir)
			.join("init.cpp")
			.createTmpPath();
		CodeGenerators::generateInitFile(classes, initH);
		File::writeFile("#include \"init.h\"\n", initCpp);
	}

	static bool ProcessFile(Path& file, Path generatedDirPath)
	{
		if (!File::isFile(file) || file.contains("generated") || File::isHidden(file) || !IsHeaderFile(file))
		{
			return false;
		}
		File::createDirIfNotExists(generatedDirPath);

		//ScriptScanner fileScanner = ScriptScanner(file);
		//std::vector<Token> fileTokens = fileScanner.ScanTokens();
		//ScriptParser fileParser = ScriptParser(fileTokens, file);
		//fileParser.Parse();
		//MergeNewClasses(fileParser.GetClasses(), file);

		std::string generatedHFilename = file.getFilenameWithoutExt() + "-generated.h";
		Path path = PathBuilder(generatedDirPath)
			.join(generatedHFilename.c_str())
			.createTmpPath();
		//File::WriteFile(fileParser.GenerateHeaderFile().c_str(), path);

		GenerateInitFiles();
		return true;
	}

	static void FileChanged(const Path& file)
	{
		const Path generatedDirPath = PathBuilder((file.getDirectory(-1) + "generated").c_str()).createPath();
		PathBuilder filePath = PathBuilder(generatedDirPath)
			.join(file);
		PathBuilder generatedFilePath = PathBuilder(rootDir)
			.join(generatedDirPath);
		if (ProcessFile(filePath.createTmpPath(), generatedFilePath.createTmpPath()))
		{
			CppBuild::build(rootDir);
		}

		String::FreeString(generatedDirPath.path);
	}

	static void GenerateInitialClassInformation(const Path& directory)
	{
		auto subDirs = File::getFoldersInDir(directory);
		for (auto dir : subDirs)
		{
			GenerateInitialClassInformation(dir);
		}

		auto files = File::getFilesInDir(directory);
		const Path generatedDir = PathBuilder(directory)
			.join("generated")
			.createPath();
		for (auto file : files)
		{
			ProcessFile(file, generatedDir);
		}

		GenerateInitFiles();

		String::FreeString(generatedDir.path);
	}

	static void GenerateBuildFile()
	{
		const Path pathToBuildScript = PathBuilder(rootDir.getDirectory(-1).c_str())
			.join("build.bat")
			.createPath();
		const Path pathToPremakeExe = PathBuilder(Settings::General::engineExeDirectory)
			.join("premake5.exe")
			.createPath();
		CodeGenerators::generateBuildFile(pathToBuildScript, pathToPremakeExe);

		String::FreeString(pathToBuildScript.path);
		String::FreeString(pathToPremakeExe.path);
	}

	void SourceFileWatcher::startFileWatcher()
	{
		if (!File::isDirectory(mRootDirectory))
		{
			Logger::Warning("'%s' is not a directory. SourceFileWatcher is not starting.", mRootDirectory.path);
			return;
		}
		rootDir = mRootDirectory;
		Logger::Log("Monitoring directory '%s'", File::getAbsolutePath(mRootDirectory).path);

		projectPremakeLua = PathBuilder(mRootDirectory.getDirectory(-1).c_str())
			.join("premake5.lua")
			.createPath();
		CodeGenerators::generatePremakeFile(projectPremakeLua);
		GenerateBuildFile();

		Logger::Log("Generating initial class information");
		GenerateInitialClassInformation(mRootDirectory);
		Logger::Log("Generating premake file %s", projectPremakeLua.path);
		CppBuild::build(rootDir);

		mFileWatcher.path = mRootDirectory;
		mFileWatcher.notifyFilters = NotifyFilters::LastAccess
			| NotifyFilters::LastWrite
			| NotifyFilters::FileName
			| NotifyFilters::DirectoryName;

		mFileWatcher.filter = "*.h";
		mFileWatcher.includeSubdirectories = true;

		mFileWatcher.onChanged = FileChanged;
		mFileWatcher.onCreated = FileChanged;
		mFileWatcher.onRenamed = FileChanged;

		mFileWatcher.start();
	}
}