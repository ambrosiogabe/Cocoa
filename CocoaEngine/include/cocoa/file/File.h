#pragma once
#include "externalLibs.h"
#include "cocoa/file/CPath.h"

namespace Cocoa
{
	struct FileHandle
	{
		const char* m_Filename = nullptr;
		char* m_Data = nullptr;
		uint32 m_Size = 0;
		bool m_Open = false;
	};

	namespace File
	{
		COCOA FileHandle* OpenFile(const CPath& filename);
		COCOA void CloseFile(FileHandle* file);
		COCOA bool WriteFile(const char* data, const CPath& filename);
		COCOA bool CreateFile(const CPath& filename, const char* extToAppend = "");
		COCOA bool DeleteFile(const CPath& filename);
		COCOA bool CopyFile(const CPath& fileToCopy, const CPath& newFileLocation, const char* newFilename = "");
		COCOA CPath GetCwd();
		COCOA CPath GetSpecialAppFolder();
		COCOA CPath GetExecutableDirectory();
		COCOA std::vector<CPath> GetFilesInDir(const CPath& directory);
		COCOA std::vector<CPath> GetFoldersInDir(const CPath& directory);
		COCOA void CreateDirIfNotExists(const CPath& directory);
		COCOA bool IsFile(const CPath& filepath);
		COCOA bool IsHidden(const CPath& filepath);
		COCOA bool IsDirectory(const CPath& directory);
		COCOA CPath GetAbsolutePath(const CPath& path);

		COCOA bool RunProgram(const CPath& pathToExe, const char* cmdArgs = "");
		COCOA bool RunProgram(const CPath& pathToExe, const std::string& cmdArgs = "");
	};
}