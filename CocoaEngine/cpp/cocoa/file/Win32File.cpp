#ifdef _WIN32
#include "cocoa/file/File.h"

#include <direct.h>
#include <shobjidl_core.h>
#include <shlobj.h>
#include <knownfolders.h>

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
		FileHandle* openFile(const Path& filename)
		{
			FileHandle* file = (FileHandle*)AllocMem(sizeof(FileHandle));
			file->filename = filename.path;

			FILE* filePointer;
			filePointer = fopen(file->filename, "rb");
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

		bool writeFile(const char* data, const Path& filename)
		{
			std::ofstream outStream(filename.path);
			outStream << data;
			outStream.close();
			return true;
		}

		bool createFile(const Path& filename, const char* extToAppend)
		{
			Path fileToWrite = filename;
			if (filename.fileExt == nullptr || filename.fileExt[0] == '\0')
			{
				StringBuilder newFilename;
				newFilename.Append(filename.path);
				newFilename.Append(extToAppend);
				fileToWrite = PathBuilder(newFilename.c_str()).createPath();
			}
			HANDLE fileHandle = CreateFileA(fileToWrite.path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			bool res = fileHandle != INVALID_HANDLE_VALUE;
			CloseHandle(fileHandle);
			return res;
		}

		bool deleteFile(const Path& filename)
		{
			return DeleteFileA(filename.path);
		}

		bool copyFile(const Path& fileToCopy, const Path& newFileLocation, const char* newFilename)
		{
			PathBuilder newFilepath = PathBuilder(newFileLocation);
			StringBuilder newFilenameWithExt;
			newFilenameWithExt.Append(newFilename);
			newFilenameWithExt.Append(fileToCopy.fileExt);
			newFilepath.join(newFilenameWithExt.c_str());
			if (!CopyFileExA(fileToCopy.path, newFilepath.c_str(), NULL, NULL, false, NULL))
			{
				Logger::Warning("Could not copy file error code: %d", GetLastError());
				return false;
			}

			return true;
		}

		Path getCwd()
		{
			char buff[FILENAME_MAX];
			char* success = _getcwd(buff, FILENAME_MAX);
			Logger::Assert(success != NULL, "Unable to get Current Working Directory.");
			return { buff };
		}

		Path getSpecialAppFolder()
		{
			PWSTR pszPath;
			if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &pszPath)))
			{
				char* tmp = (char*)AllocMem(sizeof(char) * 256);
				wcstombs(tmp, pszPath, 256);
				const Path result = PathBuilder(tmp).createPath();
				FreeMem(tmp);
				return result;
			}

			Logger::Assert(false, "Could not retrieve AppRoamingData folder.");
			return Path::createDefault();
		}

		Path getExecutableDirectory()
		{
			char filepath[MAX_PATH];
			DWORD res = GetModuleFileNameA(NULL, filepath, MAX_PATH);
			Logger::Assert(res != NULL && res != ERROR_INSUFFICIENT_BUFFER, "Get Executable Directory failed with error code: '%d'", res);

			return PathBuilder(filepath).createPath();
		}

		void createDirIfNotExists(const Path& directory)
		{
			if (!(CreateDirectoryA(directory.path, NULL) || ERROR_ALREADY_EXISTS == GetLastError()))
			{
				Logger::Assert(false, "Failed to create directory %s", directory.path);
			}
		}

		std::vector<Path> getFilesInDir(const Path& directory)
		{
			std::vector<Path> result;
			HANDLE dir;
			WIN32_FIND_DATAA fileData;

			Path dirPath = directory;
			PathBuilder wildcardMatch = dirPath;
			wildcardMatch.join("*");
			if ((dir = FindFirstFileA(wildcardMatch.c_str(), &fileData)) == INVALID_HANDLE_VALUE)
			{
				// No file found
				return result;
			}

			do
			{
				Path filename = PathBuilder(fileData.cFileName).createPath();
				PathBuilder fullFilename = dirPath;
				fullFilename.join(filename);
				const bool isDirectory = (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);

				if (filename.path[0] == '.')
				{
					continue;
				}

				if (!isDirectory)
				{
					result.push_back(fullFilename.createPath());
				}
			} while (FindNextFileA(dir, &fileData));

			FindClose(dir);

			return result;
		}

		std::vector<Path> getFoldersInDir(const Path& directory)
		{
			std::vector<Path> result;
			HANDLE dir;
			WIN32_FIND_DATAA fileData;

			Path dirPath = directory;
			PathBuilder wildcardMatch = dirPath;
			wildcardMatch.join("*");
			if ((dir = FindFirstFileA(wildcardMatch.c_str(), &fileData)) == INVALID_HANDLE_VALUE)
			{
				// No file found
				return result;
			}

			do
			{
				Path filename = PathBuilder(fileData.cFileName).createPath();
				bool isDirectory = (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);

				if (filename.path[0] == '.')
				{
					String::FreeString(filename.path);
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

		Path getAbsolutePath(const Path& path)
		{
			const int bufferLength = GetFullPathNameA(path.path, 0, nullptr, nullptr);
			char* buffer = (char*)AllocMem(sizeof(char) * bufferLength);
			GetFullPathNameA(path.path, bufferLength, buffer, nullptr);
			const Path result = PathBuilder(buffer).createPath();
			FreeMem(buffer);
			return result;
		}

		bool isFile(const Path& filepath)
		{
			const DWORD fileAttributes = GetFileAttributesA(filepath.path);
			return (fileAttributes != INVALID_FILE_ATTRIBUTES) && !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY);
		}

		bool isHidden(const Path& filepath)
		{
			const DWORD fileAttributes = GetFileAttributesA(filepath.path);
			return (fileAttributes != INVALID_FILE_ATTRIBUTES) && (fileAttributes & FILE_ATTRIBUTE_HIDDEN);
		}

		bool isDirectory(const Path& directory)
		{
			const DWORD fileAttributes = GetFileAttributesA(directory.path);
			return (fileAttributes != INVALID_FILE_ATTRIBUTES) && (fileAttributes & FILE_ATTRIBUTE_DIRECTORY);
		}

		bool runProgram(const Path& pathToExe, const char* cmdArgs)
		{
			STARTUPINFOA startupInfo;
			PROCESS_INFORMATION processInfo;

			ZeroMemory(&startupInfo, sizeof(startupInfo));
			startupInfo.cb = sizeof(startupInfo);
			ZeroMemory(&processInfo, sizeof(processInfo));

			StringBuilder cmdArguments;
			cmdArguments.Append(pathToExe.path);
			cmdArguments.Append(' ');
			cmdArguments.Append(cmdArgs);

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
				Logger::Warning("Unsuccessfully started process '%s'", pathToExe.path);
			}

			return res;
		}

		bool runProgram(const Path& pathToExe, const std::string& cmdArgs)
		{
			return runProgram(pathToExe, cmdArgs.c_str());
		}
	}
}
#endif