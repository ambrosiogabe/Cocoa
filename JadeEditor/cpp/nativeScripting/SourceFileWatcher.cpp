#include "nativeScripting/SourceFileWatcher.h"

#include "jade/util/Log.h"
#include "jade/file/IFile.h"
#include "jade/util/Settings.h"

#include "util/Settings.h"
#include "nativeScripting/ScriptScanner.h"
#include "nativeScripting/ScriptParser.h"
#include "nativeScripting/CodeGenerators.h"

namespace Jade
{
	static void RunPremake();

	static JPath rootDir = "";
	static JPath projectPremakeLua = "";
	static auto classes = std::vector<UClass>();
	static bool fileModified = false;

	SourceFileWatcher::SourceFileWatcher(JPath rootDirectory)
		: m_RootDirectory(rootDirectory)
	{
		StartFileWatcher();
	}

	const std::vector<UClass>& SourceFileWatcher::GetClasses()
	{
		return classes;
	}

	static bool IsHeaderFile(const JPath& file)
	{
		return strcmp(file.FileExt(), ".h") == 0;
	}

	static bool IsReflectionHeaderFile(const JPath& file)
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

	static void ProcessFile(JPath& file, JPath generatedDirPath, bool generateHeaders = true)
	{
		if (!IFile::IsFile(file) || strcmp(file.FileExt(), ".TMP") == 0)
		{
			return;
		}

		ScriptScanner fileScanner = ScriptScanner(file);
		std::vector<Token> fileTokens = fileScanner.ScanTokens();
		ScriptParser fileParser = ScriptParser(fileTokens, file);
		fileParser.Parse();
		// fileParser.DebugPrint();

		if (fileParser.CanGenerateHeaderFile())
		{
			auto newClasses = fileParser.GetClasses();
			for (auto clazz : newClasses)
			{
				AddClassIfNotExist(clazz);
			}

			if (generateHeaders)
			{
				JPath path = generatedDirPath + (file.GetFilenameWithoutExt() + "-generated.h");
				IFile::WriteFile(fileParser.GenerateHeaderFile().c_str(), path);
				Log::Info("Generating file at %s", path.Filepath());

				CodeGenerators::GenerateInitFile(classes, rootDir + "generated" + "init.h");
				IFile::WriteFile("#include \"init.h\"\n", rootDir + "generated" + "init.cpp");
				Log::Info("Generating init file at %s", (rootDir + "generated" + "init-tmp.h").Filepath());
				RunPremake();
			}
		}
	}

	static void Preprocess(const JPath& directoryPath)
	{
		if (directoryPath.Contains("generated"))
		{
			// Return if we are in /generated path
			return;
		}

		JPath generatedDirPath = directoryPath + "generated";
		for (JPath& file : IFile::GetFilesInDir(generatedDirPath))
		{
			if (IsHeaderFile(file) && !IsReflectionHeaderFile(file))
			{
				IFile::CreateDirIfNotExists(generatedDirPath);

				ProcessFile(file, generatedDirPath);
			}
		}

		for (JPath& directory : IFile::GetFoldersInDir(generatedDirPath))
		{
			if (directory.Contains("generated"))
			{
				Preprocess(directory);
			}
		}
	}

	static void RunPremake()
	{
		JPath pathToPremake = Settings::General::s_EngineExeDirectory + "premake5.exe";
		std::string cmdArgs = "vs2019 --file=\"" + std::string(projectPremakeLua.Filepath()) + "\"";
		IFile::RunProgram(pathToPremake, cmdArgs);
	}

	static void FileChanged(const JPath& file)
	{
		if (!file.Contains("generated"))
		{
			JPath generatedDirPath = file.GetDirectory(-1) + "generated";
			ProcessFile(rootDir + JPath(file), rootDir + generatedDirPath);
		}
	}

	static void GenerateInitialClassInformation(const JPath& directory)
	{
		auto subDirs = IFile::GetFoldersInDir(directory);
		for (auto dir : subDirs)
		{
			GenerateInitialClassInformation(dir);
		}

		auto files = IFile::GetFilesInDir(directory);
		for (auto file : files)
		{
			ProcessFile(file, directory, false);
		}
	}

	void SourceFileWatcher::StartFileWatcher()
	{
		if (!IFile::IsDirectory(m_RootDirectory))
		{
			Log::Warning("'%s' is not a directory. SourceFileWatcher is not starting.", m_RootDirectory.Filepath());
			return;
		}
		rootDir = m_RootDirectory;
		Log::Info("Monitoring directory '%s'", m_RootDirectory.Filepath());

		projectPremakeLua = JPath(m_RootDirectory.GetDirectory(-1)) + "/premake5.lua";
		CodeGenerators::GeneratePremakeFile(projectPremakeLua);
		Log::Info("Generating premake file %s", projectPremakeLua.Filepath());
		RunPremake();
		Log::Info("Generating initial class information");
		GenerateInitialClassInformation(m_RootDirectory);

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