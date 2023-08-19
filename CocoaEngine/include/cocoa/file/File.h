#ifndef COCOA_ENGINE_FILE_H
#define COCOA_ENGINE_FILE_H
#include "externalLibs.h"

#include <filesystem>

namespace Cocoa
{
	struct FileHandle
	{
		std::string filename = "";
		char* data = nullptr;
		uint32 size = 0;
		bool open = false;
	};

	namespace File
	{
		COCOA FileHandle* openFile(const std::filesystem::path& filename);
		COCOA void closeFile(FileHandle* file);
		COCOA bool writeFile(const char* data, const std::filesystem::path& filename);
		COCOA bool createFile(const std::filesystem::path& filename, const char* extToAppend = "");
		COCOA bool deleteFile(const std::filesystem::path& filename);
		COCOA bool copyFile(const std::filesystem::path& fileToCopy, const std::filesystem::path& newFileLocation, const char* newFilename = "");
		COCOA std::filesystem::path getCwd();
		COCOA std::filesystem::path getSpecialAppFolder();
		COCOA std::filesystem::path getExecutableDirectory();
		COCOA std::vector<std::filesystem::path> getFilesInDir(const std::filesystem::path& directory);
		COCOA std::vector<std::filesystem::path> getFoldersInDir(const std::filesystem::path& directory);
		COCOA void createDirIfNotExists(const std::filesystem::path& directory);
		COCOA bool isFile(const std::filesystem::path& filepath);
		COCOA bool isHidden(const std::filesystem::path& filepath);
		COCOA bool isDirectory(const std::filesystem::path& directory);
		COCOA std::filesystem::path getAbsolutePath(const std::filesystem::path& path);

		COCOA bool runProgram(const std::filesystem::path& pathToExe, const char* cmdArgs = "");
		COCOA bool runProgram(const std::filesystem::path& pathToExe, const std::string& cmdArgs = "");
	};
}

#endif
