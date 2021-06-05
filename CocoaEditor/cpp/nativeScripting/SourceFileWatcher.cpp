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
	static CPath rootDir = CPath::create();
	static CPath projectPremakeLua = CPath::create();
	static auto classes = std::vector<UClass>();
	static bool fileModified = false;
	static bool buildingCode = false;

	// Internal functions
	static void GenerateBuildFile();

	SourceFileWatcher::SourceFileWatcher(CPath rootDirectory)
		: mRootDirectory(rootDirectory)
	{
		startFileWatcher();
	}

	static bool IsHeaderFile(const CPath& file)
	{
		return strcmp(file.fileExt(), ".h") == 0 || strcmp(file.fileExt(), ".hpp") == 0;
	}

	static bool IsReflectionHeaderFile(const CPath& file)
	{
		return strcmp(file.filename(), "Reflection") == 0;
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

	static void MergeNewClasses(std::vector<UClass>& classesToMerge, const CPath& filepath)
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
		CPath generatedDir = rootDir;
		generatedDir.join(CPath::create("generated"));
		File::createDirIfNotExists(generatedDir);

		CPath initH = generatedDir;
		initH.join(CPath::create("init.h"));
		CPath initCpp = generatedDir;
		initCpp.join(CPath::create("init.pp"));
		CodeGenerators::generateInitFile(classes, initH);
		File::writeFile("#include \"init.h\"\n", initCpp);
	}

	static bool ProcessFile(CPath& file, CPath generatedDirPath)
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
		CPath path = generatedDirPath;
		path.join(CPath::create(generatedHFilename));
		//File::WriteFile(fileParser.GenerateHeaderFile().c_str(), path);

		GenerateInitFiles();
		return true;
	}

	static void FileChanged(const CPath& file)
	{
		CPath generatedDirPath = CPath::create(file.getDirectory(-1) + "generated");
		CPath filePath = generatedDirPath;
		filePath.join(file);
		CPath generatedFilePath = rootDir;
		rootDir.join(generatedDirPath);
		if (ProcessFile(filePath, generatedFilePath))
		{
			CppBuild::build(rootDir);
		}
	}

	static void GenerateInitialClassInformation(const CPath& directory)
	{
		auto subDirs = File::getFoldersInDir(directory);
		for (auto dir : subDirs)
		{
			GenerateInitialClassInformation(dir);
		}

		auto files = File::getFilesInDir(directory);
		CPath generatedDir = directory;
		generatedDir.join(CPath::create("generated"));
		for (auto file : files)
		{
			ProcessFile(file, generatedDir);
		}

		GenerateInitFiles();
	}

	static void GenerateBuildFile()
	{
		CPath pathToBuildScript = CPath::create(rootDir.getDirectory(-1));
		pathToBuildScript.join(CPath::create("build.bat"));
		CPath pathToPremakeExe = Settings::General::engineExeDirectory;
		pathToPremakeExe.join(CPath::create("premake5.exe"));
		CodeGenerators::generateBuildFile(pathToBuildScript, pathToPremakeExe);
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

		projectPremakeLua = CPath::create(mRootDirectory.getDirectory(-1));
		projectPremakeLua.join(CPath::create("premake5.lua"));
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