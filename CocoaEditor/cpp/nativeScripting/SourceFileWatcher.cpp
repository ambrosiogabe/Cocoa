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
		: m_RootDirectory(rootDirectory)
	{
		StartFileWatcher();
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
			if (c.m_ClassName == clazz.m_ClassName)
			{
				classes[i].m_ClassName = clazz.m_ClassName;
				classes[i].m_FullFilepath = clazz.m_FullFilepath;
				classes[i].m_Variables = clazz.m_Variables;
				return;
			}
		}

		classes.push_back(clazz);
	}

	static std::vector<UClass>::iterator FindClass(UClass& c, std::vector<UClass>& classesToSearch)
	{
		for (auto classIter = classesToSearch.begin(); classIter != classesToSearch.end(); classIter++)
		{
			if (classIter->m_ClassName == c.m_ClassName)
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
				c->m_ClassName = clazz.m_ClassName;
				c->m_FullFilepath = clazz.m_FullFilepath;
				c->m_Variables = clazz.m_Variables;
			}
			else
			{
				classes.push_back(clazz);
			}
		}

		for (auto classIter = classes.begin(); classIter != classes.end(); classIter++)
		{
			if (classIter->m_FullFilepath == filepath)
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
		CodeGenerators::GenerateInitFile(classes, initH);
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
			CppBuild::Build(rootDir);
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
		CodeGenerators::GenerateBuildFile(pathToBuildScript, pathToPremakeExe);
	}

	void SourceFileWatcher::StartFileWatcher()
	{
		if (!File::isDirectory(m_RootDirectory))
		{
			Logger::Warning("'%s' is not a directory. SourceFileWatcher is not starting.", m_RootDirectory.path);
			return;
		}
		rootDir = m_RootDirectory;
		Logger::Log("Monitoring directory '%s'", File::getAbsolutePath(m_RootDirectory).path);

		projectPremakeLua = CPath::create(m_RootDirectory.getDirectory(-1));
		projectPremakeLua.join(CPath::create("premake5.lua"));
		CodeGenerators::GeneratePremakeFile(projectPremakeLua);
		GenerateBuildFile();

		Logger::Log("Generating initial class information");
		GenerateInitialClassInformation(m_RootDirectory);
		Logger::Log("Generating premake file %s", projectPremakeLua.path);
		CppBuild::Build(rootDir);

		m_FileWatcher.path = m_RootDirectory;
		m_FileWatcher.notifyFilters = NotifyFilters::LastAccess
			| NotifyFilters::LastWrite
			| NotifyFilters::FileName
			| NotifyFilters::DirectoryName;

		m_FileWatcher.filter = "*.h";
		m_FileWatcher.includeSubdirectories = true;

		m_FileWatcher.onChanged = FileChanged;
		m_FileWatcher.onCreated = FileChanged;
		m_FileWatcher.onRenamed = FileChanged;

		m_FileWatcher.start();
	}
}