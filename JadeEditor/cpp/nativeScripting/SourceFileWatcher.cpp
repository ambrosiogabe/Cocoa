#include "nativeScripting/SourceFileWatcher.h"

#include "jade/util/Log.h"
#include "jade/file/IFile.h"

#include "nativeScripting/ScriptScanner.h"
#include "nativeScripting/ScriptParser.h"

namespace Jade
{
	static JPath rootDir = "";

	SourceFileWatcher::SourceFileWatcher(JPath rootDirectory)
		: m_RootDirectory(rootDirectory)
	{
		StartFileWatcher();
	}

	static bool IsHeaderFile(const JPath& file)
	{
		return strcmp(file.FileExt(), ".h") == 0;
	}

	static bool IsReflectionHeaderFile(const JPath& file)
	{
		return strcmp(file.Filename(), "Reflection") == 0;
	}

	static void ProcessFile(JPath& file)
	{
		if (!IFile::IsFile(file))
		{
			return;
		}

		Log::Info("Processing file '%s'", file.Filepath());
		ScriptScanner fileScanner = ScriptScanner(file);
		std::list<Token> fileTokens = fileScanner.ScanTokens();
		ScriptParser fileParser = ScriptParser(fileTokens, file);
		fileParser.Parse();
		fileParser.DebugPrint();
		//Classes.AddRange(fileParser.GetClasses());

		//if (fileParser.CanGenerateHeaderFile())
		//{
		//	File.WriteAllText(Path.Combine(generatedDirPath, Path.GetFileNameWithoutExtension(file) + "-generated.h"), fileParser.GenerateHeaderFile());
		//}
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

				ProcessFile(file);// , generatedDirPath);
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

	static void FileChanged(const JPath& file)
	{
		Log::Info("File changed '%s'", file.Filepath());
		if (!file.Contains("generated"))
		{
			//Classes.Clear();
			//Preprocess(Path.GetDirectoryName(e.FullPath));
			ProcessFile(rootDir + JPath(file));
			//InitFileGenerator.GenerateInitFile(WatchDirectory, Classes);
			//PremakeGenerator.GeneratePremakeFile(ProjectPremakeLua);
			//RunPremake();
			//Compile();
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