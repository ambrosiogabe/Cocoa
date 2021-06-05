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
		FileHandle* openFile(const CPath& filename)
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

		bool writeFile(const char* data, const CPath& filename)
		{
			std::ofstream outStream(filename.path);
			outStream << data;
			outStream.close();
			return true;
		}

		bool createFile(const CPath& filename, const char* extToAppend)
		{
			CPath fileToWrite = filename;
			if (filename.fileExt() == nullptr || filename.fileExt()[0] == '\0')
			{
				fileToWrite = CPath::create(filename.path + std::string(extToAppend));
			}
			HANDLE fileHandle = CreateFileA(fileToWrite.path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			bool res = fileHandle != INVALID_HANDLE_VALUE;
			CloseHandle(fileHandle);
			return res;
		}

		bool deleteFile(const CPath& filename)
		{
			return DeleteFileA(filename.path);
		}

		bool copyFile(const CPath& fileToCopy, const CPath& newFileLocation, const char* newFilename)
		{
			CPath newFilepath = newFileLocation;
			newFilepath.join(CPath::create(std::string(newFilename) + fileToCopy.fileExt()));
			if (!CopyFileExA(fileToCopy.path, newFilepath.path, NULL, NULL, false, NULL))
			{
				Logger::Warning("Could not copy file error code: %d", GetLastError());
				return false;
			}

			return true;
		}

		CPath getCwd()
		{
			char buff[FILENAME_MAX];
			char* success = _getcwd(buff, FILENAME_MAX);
			Logger::Assert(success != NULL, "Unable to get Current Working Directory.");
			return { buff };
		}

		CPath getSpecialAppFolder()
		{
			PWSTR pszPath;
			if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &pszPath)))
			{
				char* tmp = (char*)AllocMem(sizeof(char) * 256);
				wcstombs(tmp, pszPath, 256);
				CPath result = CPath::create(tmp);
				FreeMem(tmp);
				return result;
			}

			Logger::Assert(false, "Could not retrieve AppRoamingData folder.");
			return CPath::create();
		}

		CPath getExecutableDirectory()
		{
			char filepath[MAX_PATH];
			DWORD res = GetModuleFileNameA(NULL, filepath, MAX_PATH);
			Logger::Assert(res != NULL && res != ERROR_INSUFFICIENT_BUFFER, "Get Executable Directory failed with error code: '%d'", res);

			return CPath::create(filepath);
		}

		void createDirIfNotExists(const CPath& directory)
		{
			if (!(CreateDirectoryA(directory.path, NULL) || ERROR_ALREADY_EXISTS == GetLastError()))
			{
				Logger::Assert(false, "Failed to create directory %s", directory.path);
			}
		}

		std::vector<CPath> getFilesInDir(const CPath& directory)
		{
			std::vector<CPath> result;
			HANDLE dir;
			WIN32_FIND_DATAA fileData;

			CPath dirPath = directory;
			CPath wildcardMatch = dirPath;
			wildcardMatch.join(CPath::create("*"));
			if ((dir = FindFirstFileA(wildcardMatch.path, &fileData)) == INVALID_HANDLE_VALUE)
			{
				// No file found
				return result;
			}

			do
			{
				CPath filename = CPath::create(fileData.cFileName);
				CPath fullFilename = dirPath;
				fullFilename.join(filename);
				const bool isDirectory = (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);

				if (filename.path[0] == '.')
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

		std::vector<CPath> getFoldersInDir(const CPath& directory)
		{
			std::vector<CPath> result;
			HANDLE dir;
			WIN32_FIND_DATAA fileData;

			CPath dirPath = directory;
			CPath wildcardMatch = dirPath;
			wildcardMatch.join(CPath::create("*"));
			if ((dir = FindFirstFileA(wildcardMatch.path, &fileData)) == INVALID_HANDLE_VALUE)
			{
				// No file found
				return result;
			}

			do
			{
				CPath filename = CPath::create(fileData.cFileName);
				bool isDirectory = (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);

				if (filename.path[0] == '.')
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

		CPath getAbsolutePath(const CPath& path)
		{
			int bufferLength = GetFullPathNameA(path.path, 0, NULL, NULL);
			char* buffer = (char*)AllocMem(sizeof(char) * bufferLength);
			GetFullPathNameA(path.path, bufferLength, buffer, NULL);
			CPath result = CPath::create(buffer);
			FreeMem(buffer);
			return result;
		}

		bool isFile(const CPath& filepath)
		{
			DWORD fileAttributes = GetFileAttributesA(filepath.path);
			return (fileAttributes != INVALID_FILE_ATTRIBUTES) && !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY);
		}

		bool isHidden(const CPath& filepath)
		{
			DWORD fileAttributes = GetFileAttributesA(filepath.path);
			return (fileAttributes != INVALID_FILE_ATTRIBUTES) && (fileAttributes & FILE_ATTRIBUTE_HIDDEN);
		}

		bool isDirectory(const CPath& filepath)
		{
			DWORD fileAttributes = GetFileAttributesA(filepath.path);
			return (fileAttributes != INVALID_FILE_ATTRIBUTES) && (fileAttributes & FILE_ATTRIBUTE_DIRECTORY);
		}

		bool runProgram(const CPath& pathToExe, const char* cmdArguments)
		{
			STARTUPINFOA startupInfo;
			PROCESS_INFORMATION processInfo;

			ZeroMemory(&startupInfo, sizeof(startupInfo));
			startupInfo.cb = sizeof(startupInfo);
			ZeroMemory(&processInfo, sizeof(processInfo));

			std::string cmdArgs = pathToExe.path + std::string(" ") + cmdArguments;

			// Start the program
			bool res = CreateProcessA(
				NULL,  // Application Name
				(LPSTR)cmdArgs.c_str(),   // Command Line Args
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

		bool runProgram(const CPath& pathToExe, const std::string& cmdArgs)
		{
			return runProgram(pathToExe, cmdArgs.c_str());
		}
	}
}
#endif