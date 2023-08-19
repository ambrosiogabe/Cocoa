#ifdef _WIN32
#include "cocoa/file/File.h"

#include <direct.h>
#include <shobjidl_core.h>
#include <shlobj.h>
#include <knownfolders.h>

#include <filesystem>

#ifdef CopyFile
#undef CopyFile
#endif
#ifdef CreateFile
#undef CreateFile
#endif
#ifdef DeleteFile
#undef DeleteFile
#endif

namespace Cocoa
{
	namespace File
	{
		FileHandle* openFile(const std::filesystem::path& filename)
		{
			FileHandle* file = (FileHandle*)AllocMem(sizeof(FileHandle));
			new(file)FileHandle();
			file->filename = filename.filename().string();

			FILE* filePointer;
			filePointer = fopen(file->filename.c_str(), "rb");
			if (filePointer)
			{
				fseek(filePointer, 0, SEEK_END);
				file->size = ftell(filePointer);
				rewind(filePointer);
				
				file->data = (char*)AllocMem(sizeof(char) * (file->size + 1));
				if (!file->data)
				{
					fclose(filePointer);
					Logger::Warning("Memory allocation failed.");
					return file;
				}

				int elementsRead = fread(file->data, file->size, 1, filePointer);
				if (elementsRead != 1)
				{
					fclose(filePointer);
					FreeMem(file->data);
					file->data = nullptr;
					Logger::Warning("Failed to read file properly.");
					return file;
				}

				file->data[file->size] = '\0';
				fclose(filePointer);
			}
			else
			{
				file->data = nullptr;
				file->open = false;
				file->size = 0;
			}

			return file;
		}

		void closeFile(FileHandle* file)
		{
			if (file)
			{
				if (file->data)
				{
					FreeMem(file->data);
					file->data = nullptr;
				}
				else
				{
					Logger::Warning("Tried to free invalid file.");
				}
				FreeMem(file);
				file = nullptr;
			}
			else
			{
				Logger::Warning("Tried to free invalid file.");
			}
		}

		bool writeFile(const char* data, const std::filesystem::path& filename)
		{
			std::ofstream outStream(filename.string());
			outStream << data;
			outStream.close();
			return true;
		}

		bool createFile(const std::filesystem::path& filename, const char* extToAppend)
		{
			std::filesystem::path fileToWrite = filename;
			if (!filename.has_extension())
			{
				fileToWrite = (filename.string() + extToAppend);
			}
			HANDLE fileHandle = CreateFileA(fileToWrite.string().c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			bool res = fileHandle != INVALID_HANDLE_VALUE;
			CloseHandle(fileHandle);
			return res;
		}

		bool deleteFile(const std::filesystem::path& filename)
		{
			return DeleteFileA(filename.string().c_str());
		}

		bool copyFile(const std::filesystem::path& fileToCopy, const std::filesystem::path& newFileLocation, const char* newFilename)
		{
			std::string newFilenameWithExt = newFilename + std::string(fileToCopy.extension().string());
			std::filesystem::path newFilepath = newFileLocation/newFilenameWithExt;
			if (!CopyFileExA(fileToCopy.string().c_str(), newFilepath.string().c_str(), NULL, NULL, false, NULL))
			{
				Logger::Warning("Could not copy file error code: %d", GetLastError());
				return false;
			}

			return true;
		}

		std::filesystem::path getCwd()
		{
			char buff[FILENAME_MAX];
			char* success = _getcwd(buff, FILENAME_MAX);
			Logger::Assert(success != NULL, "Unable to get Current Working Directory.");
			return std::filesystem::path(std::string(buff));
		}

		std::filesystem::path getSpecialAppFolder()
		{
			PWSTR pszPath;
			if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &pszPath)))
			{
				char* tmp = (char*)AllocMem(sizeof(char) * 256);
				wcstombs(tmp, pszPath, 256);
				std::filesystem::path result = std::filesystem::path(std::string(tmp));
				FreeMem(tmp);
				return result;
			}

			Logger::Assert(false, "Could not retrieve AppRoamingData folder.");
			return {};
		}

		std::filesystem::path getExecutableDirectory()
		{
			char filepath[MAX_PATH];
			DWORD res = GetModuleFileNameA(NULL, filepath, MAX_PATH);
			Logger::Assert(res != NULL && res != ERROR_INSUFFICIENT_BUFFER, "Get Executable Directory failed with error code: '%d'", res);

			return std::filesystem::path(std::string(filepath));
		}

		void createDirIfNotExists(const std::filesystem::path& directory)
		{
			if (!(CreateDirectoryA(directory.string().c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()))
			{
				Logger::Assert(false, "Failed to create directory %s", directory.string().c_str());
			}
		}

		std::vector<std::filesystem::path> getFilesInDir(const std::filesystem::path& directory)
		{
			std::vector<std::filesystem::path> result;
			HANDLE dir;
			WIN32_FIND_DATAA fileData;

			std::filesystem::path dirPath = directory;
			std::filesystem::path wildcardMatch = dirPath/"*";
			if ((dir = FindFirstFileA(wildcardMatch.string().c_str(), &fileData)) == INVALID_HANDLE_VALUE)
			{
				// No file found
				return result;
			}

			do
			{
				std::filesystem::path filename = std::filesystem::path(fileData.cFileName);
				std::filesystem::path fullFilename = dirPath/filename;
				const bool isDirectory = (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);

				if (filename.string()[0] == '.')
				{
					continue;
				}

				if (!isDirectory)
				{
					result.push_back(fullFilename);
				}
			} while (FindNextFileA(dir, &fileData));

			FindClose(dir);

			return result;
		}

		std::vector<std::filesystem::path> getFoldersInDir(const std::filesystem::path& directory)
		{
			std::vector<std::filesystem::path> result;
			HANDLE dir;
			WIN32_FIND_DATAA fileData;

			std::filesystem::path dirPath = directory;
			std::filesystem::path wildcardMatch = dirPath/"*";
			if ((dir = FindFirstFileA(wildcardMatch.string().c_str(), &fileData)) == INVALID_HANDLE_VALUE)
			{
				// No file found
				return result;
			}

			do
			{
				std::filesystem::path filename = std::filesystem::path(fileData.cFileName);
				bool isDirectory = (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);

				if (filename.string()[0] == '.')
				{
					continue;
				}

				if (isDirectory)
				{
					result.push_back(filename);
				}
			} while (FindNextFileA(dir, &fileData));

			FindClose(dir);

			return result;
		}

		std::filesystem::path getAbsolutePath(const std::filesystem::path& path)
		{
			const int bufferLength = GetFullPathNameA(path.string().c_str(), 0, nullptr, nullptr);
			char* buffer = (char*)AllocMem(sizeof(char) * bufferLength);
			GetFullPathNameA(path.string().c_str(), bufferLength, buffer, nullptr);
			const std::filesystem::path result = std::filesystem::path(std::string(buffer));
			FreeMem(buffer);
			return result;
		}

		bool isFile(const std::filesystem::path& filepath)
		{
			const DWORD fileAttributes = GetFileAttributesA(filepath.string().c_str());
			return (fileAttributes != INVALID_FILE_ATTRIBUTES) && !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY);
		}

		bool isHidden(const std::filesystem::path& filepath)
		{
			const DWORD fileAttributes = GetFileAttributesA(filepath.string().c_str());
			return (fileAttributes != INVALID_FILE_ATTRIBUTES) && (fileAttributes & FILE_ATTRIBUTE_HIDDEN);
		}

		bool isDirectory(const std::filesystem::path& directory)
		{
			const DWORD fileAttributes = GetFileAttributesA(directory.string().c_str());
			return (fileAttributes != INVALID_FILE_ATTRIBUTES) && (fileAttributes & FILE_ATTRIBUTE_DIRECTORY);
		}

		bool runProgram(const std::filesystem::path& pathToExe, const char* cmdArgs)
		{
			STARTUPINFOA startupInfo;
			PROCESS_INFORMATION processInfo;

			ZeroMemory(&startupInfo, sizeof(startupInfo));
			startupInfo.cb = sizeof(startupInfo);
			ZeroMemory(&processInfo, sizeof(processInfo));

			std::string cmdArguments = pathToExe.string() + " " + cmdArgs;

			// Start the program
			bool res = CreateProcessA(
				NULL,  // Application Name
				(LPSTR)cmdArguments.c_str(),   // Command Line Args
				NULL,                  // Process Attributes
				NULL,                  // Thread Attributes
				FALSE,                 // Inherit Handles
				NULL,      // Creation Flags
				NULL,                  // Environment
				NULL,                  // Current Directory
				&startupInfo,          // Startup Info
				&processInfo           // Process Info
			);

			if (res)
			{
				// Close process and thread handles
				CloseHandle(processInfo.hProcess);
				CloseHandle(processInfo.hThread);
			}
			else
			{
				Logger::Warning("Unsuccessfully started process '%s'", pathToExe.string().c_str());
			}

			return res;
		}

		bool runProgram(const std::filesystem::path& pathToExe, const std::string& cmdArgs)
		{
			return runProgram(pathToExe, cmdArgs.c_str());
		}
	}
}
#endif