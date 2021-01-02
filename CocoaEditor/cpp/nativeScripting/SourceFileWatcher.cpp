#include "nativeScripting/SourceFileWatcher.h"

#include "cocoa/util/Log.h"
#include "cocoa/file/IFile.h"
#include "cocoa/util/Settings.h"

#include "util/Settings.h"
#include "nativeScripting/ScriptScanner.h"
#include "nativeScripting/ScriptParser.h"
#include "nativeScripting/CodeGenerators.h"

namespace Cocoa
{
	static void RunPremake();

	static CPath rootDir = "";
	static CPath projectPremakeLua = "";
	static auto classes = std::vector<UClass>();
	static bool fileModified = false;
	static bool runningPremake = false;

	SourceFileWatcher::SourceFileWatcher(CPath rootDirectory)
		: m_RootDirectory(rootDirectory)
	{
		StartFileWatcher();
	}

	const std::vector<UClass>& SourceFileWatcher::GetClasses()
	{
		return classes;
	}

	static bool IsSourceFile(const CPath& file)
	{
		return strcmp(file.FileExt(), ".h") == 0 || strcmp(file.FileExt(), ".hpp") == 0 || strcmp(file.FileExt(), ".cpp") == 0 || strcmp(file.FileExt(), ".c") == 0;
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
		IFile::CreateDirIfNotExists(rootDir + "generated");
		CodeGenerators::GenerateInitFile(classes, rootDir + "generated" + "init.h");
		IFile::WriteFile("#include \"init.h\"\n", rootDir + "generated" + "init.cpp");
	}

	static bool ProcessFile(CPath& file, CPath generatedDirPath)
	{
		if (!IFile::IsFile(file) || file.Contains("generated") || IFile::IsHidden(file) || !IsSourceFile(file))
		{
			return false;
		}
		IFile::CreateDirIfNotExists(generatedDirPath);

		ScriptScanner fileScanner = ScriptScanner(file);
		std::vector<Token> fileTokens = fileScanner.ScanTokens();
		ScriptParser fileParser = ScriptParser(fileTokens, file);
		fileParser.Parse();
		MergeNewClasses(fileParser.GetClasses(), file);

		CPath path = generatedDirPath + (file.GetFilenameWithoutExt() + "-generated.h");
		IFile::WriteFile(fileParser.GenerateHeaderFile().c_str(), path);

		GenerateInitFiles();
		return true;
	}


	static void RunPremake()
	{
		if (!runningPremake)
		{
			runningPremake = true;
			CPath pathToPremake = Settings::General::s_EngineExeDirectory + "premake5.exe";
			std::string cmdArgs = "vs2019 --file=\"" + std::string(projectPremakeLua.Filepath()) + "\"";
			IFile::RunProgram(pathToPremake, cmdArgs);
			runningPremake = false;
		}
	}

	static void FileChanged(const CPath& file)
	{
		CPath generatedDirPath = file.GetDirectory(-1) + "generated";
		if (ProcessFile(rootDir + CPath(file), rootDir + generatedDirPath))
		{
			RunPremake();
		}
	}

	static void GenerateInitialClassInformation(const CPath& directory)
	{
		auto subDirs = IFile::GetFoldersInDir(directory);
		for (auto dir : subDirs)
		{
			GenerateInitialClassInformation(dir);
		}

		auto files = IFile::GetFilesInDir(directory);
		CPath generatedDir = directory + "generated";
		for (auto file : files)
		{
			ProcessFile(file, generatedDir);
		}

		GenerateInitFiles();
	}

	void SourceFileWatcher::StartFileWatcher()
	{
		if (!IFile::IsDirectory(m_RootDirectory))
		{
			Log::Warning("'%s' is not a directory. SourceFileWatcher is not starting.", m_RootDirectory.Filepath());
			return;
		}
		rootDir = m_RootDirectory;
		Log::Log("Monitoring directory '%s'", IFile::GetAbsolutePath(m_RootDirectory.Filepath()).Filepath());

		projectPremakeLua = CPath(m_RootDirectory.GetDirectory(-1)) + "/premake5.lua";
		CodeGenerators::GeneratePremakeFile(projectPremakeLua);

		Log::Log("Generating initial class information");
		GenerateInitialClassInformation(m_RootDirectory);
		Log::Log("Generating premake file %s", projectPremakeLua.Filepath());
		RunPremake();

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