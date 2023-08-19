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
	static std::filesystem::path rootDir = "";
	static std::filesystem::path projectPremakeLua = "";
	static auto classes = std::vector<UClass>();
	static bool fileModified = false;
	static bool buildingCode = false;

	// Internal functions
	static void GenerateBuildFile();

	SourceFileWatcher::SourceFileWatcher(const std::filesystem::path& rootDirectory)
		: mRootDirectory(rootDirectory)
	{
		startFileWatcher();
	}

	static bool IsHeaderFile(const std::filesystem::path& file)
	{
		return file.extension().string() == ".h" || file.extension().string() == ".hpp";
	}

	static bool IsReflectionHeaderFile(const std::filesystem::path& file)
	{
		return file.filename().string() == "Reflection";
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

	static void MergeNewClasses(std::vector<UClass>& classesToMerge, const std::filesystem::path& filepath)
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
		std::filesystem::path generatedDir = rootDir / "generated";
		File::createDirIfNotExists(generatedDir);

		std::filesystem::path initH = generatedDir / "init.h";
		std::filesystem::path initCpp = generatedDir / "init.cpp";
		CodeGenerators::generateInitFile(classes, initH);
		File::writeFile("#include \"init.h\"\n", initCpp);
	}

	static bool ProcessFile(std::filesystem::path& file, const std::filesystem::path& generatedDirPath)
	{
		if (!File::isFile(file) || File::isHidden(file) || !IsHeaderFile(file))
		{
			return false;
		}
		File::createDirIfNotExists(generatedDirPath);

		//ScriptScanner fileScanner = ScriptScanner(file);
		//std::vector<Token> fileTokens = fileScanner.ScanTokens();
		//ScriptParser fileParser = ScriptParser(fileTokens, file);
		//fileParser.Parse();
		//MergeNewClasses(fileParser.GetClasses(), file);

		std::string generatedHFilename = file.filename().string() + "-generated.h";
		std::filesystem::path path = generatedDirPath / generatedHFilename;
		//File::WriteFile(fileParser.GenerateHeaderFile().c_str(), path);

		GenerateInitFiles();
		return true;
	}

	static void FileChanged(const std::filesystem::path& file)
	{
		const std::filesystem::path generatedDirPath = file.parent_path().string() + "generated";
		std::filesystem::path filePath = generatedDirPath / file;
		std::filesystem::path generatedFilePath = rootDir / generatedDirPath;
		if (ProcessFile(filePath, generatedFilePath))
		{
			CppBuild::build(rootDir);
		}
	}

	static void GenerateInitialClassInformation(const std::filesystem::path& directory)
	{
		auto subDirs = File::getFoldersInDir(directory);
		for (auto dir : subDirs)
		{
			GenerateInitialClassInformation(dir);
		}

		auto files = File::getFilesInDir(directory);
		const std::filesystem::path generatedDir = directory / "generated";
		for (auto file : files)
		{
			ProcessFile(file, generatedDir);
		}

		GenerateInitFiles();
	}

	static void GenerateBuildFile()
	{
		const std::filesystem::path pathToBuildScript = rootDir.parent_path() / "build.bat";
		const std::filesystem::path pathToPremakeExe = Settings::General::engineExeDirectory / "premake5.exe";
		CodeGenerators::generateBuildFile(pathToBuildScript, pathToPremakeExe);
	}

	void SourceFileWatcher::startFileWatcher()
	{
		if (!File::isDirectory(mRootDirectory))
		{
			Logger::Warning("'%s' is not a directory. SourceFileWatcher is not starting.", mRootDirectory.string().c_str());
			return;
		}
		rootDir = mRootDirectory;
		Logger::Log("Monitoring directory '%s'", File::getAbsolutePath(mRootDirectory).c_str());

		projectPremakeLua = mRootDirectory.parent_path() / "premake5.lua";
		CodeGenerators::generatePremakeFile(projectPremakeLua);
		GenerateBuildFile();

		Logger::Log("Generating initial class information");
		GenerateInitialClassInformation(mRootDirectory);
		Logger::Log("Generating premake file %s", projectPremakeLua.string().c_str());
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