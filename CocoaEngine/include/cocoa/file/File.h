#ifndef COCOA_ENGINE_FILE_H
#define COCOA_ENGINE_FILE_H
#include "externalLibs.h"
#include "cocoa/file/Path.h"

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
		COCOA FileHandle* openFile(const Path& filename);
		COCOA void closeFile(FileHandle* file);
		COCOA bool writeFile(const char* data, const Path& filename);
		COCOA bool createFile(const Path& filename, const char* extToAppend = "");
		COCOA bool deleteFile(const Path& filename);
		COCOA bool copyFile(const Path& fileToCopy, const Path& newFileLocation, const char* newFilename = "");
		COCOA Path getCwd();
		COCOA Path getSpecialAppFolder();
		COCOA Path getExecutableDirectory();
		COCOA std::vector<Path> getFilesInDir(const Path& directory);
		COCOA std::vector<Path> getFoldersInDir(const Path& directory);
		COCOA void createDirIfNotExists(const Path& directory);
		COCOA bool isFile(const Path& filepath);
		COCOA bool isHidden(const Path& filepath);
		COCOA bool isDirectory(const Path& directory);
		COCOA Path getAbsolutePath(const Path& path);

		COCOA bool runProgram(const Path& pathToExe, const char* cmdArgs = "");
		COCOA bool runProgram(const Path& pathToExe, const std::string& cmdArgs = "");
	};
}

#endif
