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
	static CPath rootDir = CPath::Create();
	static CPath projectPremakeLua = CPath::Create();
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
		return strcmp(file.FileExt(), ".h") == 0 || strcmp(file.FileExt(), ".hpp") == 0;
	}

	static bool IsReflectionHeaderFile(const CPath& file)
	{
		return strcmp(file.Filename(), "Reflection") == 0;
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
		generatedDir.Join(CPath::Create("generated"));
		File::CreateDirIfNotExists(generatedDir);

		CPath initH = generatedDir;
		initH.Join(CPath::Create("init.h"));
		CPath initCpp = generatedDir;
		initCpp.Join(CPath::Create("init.pp"));
		CodeGenerators::GenerateInitFile(classes, initH);
		File::WriteFile("#include \"init.h\"\n", initCpp);
	}

	static bool ProcessFile(CPath& file, CPath generatedDirPath)
	{
		if (!File::IsFile(file) || file.Contains("generated") || File::IsHidden(file) || !IsHeaderFile(file))
		{
			return false;
		}
		File::CreateDirIfNotExists(generatedDirPath);

		//ScriptScanner fileScanner = ScriptScanner(file);
		//std::vector<Token> fileTokens = fileScanner.ScanTokens();
		//ScriptParser fileParser = ScriptParser(fileTokens, file);
		//fileParser.Parse();
		//MergeNewClasses(fileParser.GetClasses(), file);

		std::string generatedHFilename = file.GetFilenameWithoutExt() + "-generated.h";
		CPath path = generatedDirPath;
		path.Join(CPath::Create(generatedHFilename));
		//File::WriteFile(fileParser.GenerateHeaderFile().c_str(), path);

		GenerateInitFiles();
		return true;
	}

	static void FileChanged(const CPath& file)
	{
		CPath generatedDirPath = CPath::Create(file.GetDirectory(-1) + "generated");
		CPath filePath = generatedDirPath;
		filePath.Join(file);
		CPath generatedFilePath = rootDir;
		rootDir.Join(generatedDirPath);
		if (ProcessFile(filePath, generatedFilePath))
		{
			CppBuild::Build(rootDir);
		}
	}

	static void GenerateInitialClassInformation(const CPath& directory)
	{
		auto subDirs = File::GetFoldersInDir(directory);
		for (auto dir : subDirs)
		{
			GenerateInitialClassInformation(dir);
		}

		auto files = File::GetFilesInDir(directory);
		CPath generatedDir = directory;
		generatedDir.Join(CPath::Create("generated"));
		for (auto file : files)
		{
			ProcessFile(file, generatedDir);
		}

		GenerateInitFiles();
	}

	static void GenerateBuildFile()
	{
		CPath pathToBuildScript = CPath::Create(rootDir.GetDirectory(-1));
		pathToBuildScript.Join(CPath::Create("build.bat"));
		CPath pathToPremakeExe = Settings::General::s_EngineExeDirectory;
		pathToPremakeExe.Join(CPath::Create("premake5.exe"));
		CodeGenerators::GenerateBuildFile(pathToBuildScript, pathToPremakeExe);
	}

	void SourceFileWatcher::StartFileWatcher()
	{
		if (!File::IsDirectory(m_RootDirectory))
		{
			Logger::Warning("'%s' is not a directory. SourceFileWatcher is not starting.", m_RootDirectory.Path);
			return;
		}
		rootDir = m_RootDirectory;
		Logger::Log("Monitoring directory '%s'", File::GetAbsolutePath(m_RootDirectory).Path);

		projectPremakeLua = CPath::Create(m_RootDirectory.GetDirectory(-1));
		projectPremakeLua.Join(CPath::Create("premake5.lua"));
		CodeGenerators::GeneratePremakeFile(projectPremakeLua);
		GenerateBuildFile();

		Logger::Log("Generating initial class information");
		GenerateInitialClassInformation(m_RootDirectory);
		Logger::Log("Generating premake file %s", projectPremakeLua.Path);
		CppBuild::Build(rootDir);

		m_FileWatcher.m_Path = m_RootDirectory;
		m_FileWatcher.m_NotifyFilters = NotifyFilters::LastAccess
			| NotifyFilters::LastWrite
			| NotifyFilters::FileName
			| NotifyFilters::DirectoryName;

		m_FileWatcher.m_Filter = "*.h";
		m_FileWatcher.m_IncludeSubdirectories = true;

		m_FileWatcher.m_OnChanged = FileChanged;
		m_FileWatcher.m_OnCreated = FileChanged;
		m_FileWatcher.m_OnRenamed = FileChanged;

		m_FileWatcher.Start();
	}
}