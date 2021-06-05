#ifndef COCOA_ENGINE_FILE_H
#define COCOA_ENGINE_FILE_H
#include "externalLibs.h"
#include "cocoa/file/CPath.h"

namespace Cocoa
{
	struct FileHandle
	{
		const char* filename = nullptr;
		char* data = nullptr;
		uint32 size = 0;
		bool open = false;
	};

	namespace File
	{
		COCOA FileHandle* openFile(const CPath& filename);
		COCOA void closeFile(FileHandle* file);
		COCOA bool writeFile(const char* data, const CPath& filename);
		COCOA bool createFile(const CPath& filename, const char* extToAppend = "");
		COCOA bool deleteFile(const CPath& filename);
		COCOA bool copyFile(const CPath& fileToCopy, const CPath& newFileLocation, const char* newFilename = "");
		COCOA CPath getCwd();
		COCOA CPath getSpecialAppFolder();
		COCOA CPath getExecutableDirectory();
		COCOA std::vector<CPath> getFilesInDir(const CPath& directory);
		COCOA std::vector<CPath> getFoldersInDir(const CPath& directory);
		COCOA void createDirIfNotExists(const CPath& directory);
		COCOA bool isFile(const CPath& filepath);
		COCOA bool isHidden(const CPath& filepath);
		COCOA bool isDirectory(const CPath& directory);
		COCOA CPath getAbsolutePath(const CPath& path);

		COCOA bool runProgram(const CPath& pathToExe, const char* cmdArgs = "");
		COCOA bool runProgram(const CPath& pathToExe, const std::string& cmdArgs = "");
	};
}

#endif
