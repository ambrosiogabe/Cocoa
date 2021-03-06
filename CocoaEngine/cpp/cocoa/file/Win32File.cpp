#ifdef _WIN32
#include "cocoa/file/File.h"
#include "cocoa/util/Log.h"
#include "cocoa/core/Memory.h"

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
		FileHandle* OpenFile(const CPath& filename)
		{
			FileHandle* file = (FileHandle*)AllocMem(sizeof(FileHandle));
			file->m_Filename = filename.Path.c_str();

			FILE* filePointer;
			filePointer = fopen(file->m_Filename, "rb");
			if (filePointer)
			{
				fseek(filePointer, 0, SEEK_END);
				file->m_Size = ftell(filePointer);
				rewind(filePointer);
				
				file->m_Data = (char*)AllocMem(sizeof(char) * (file->m_Size + 1));
				if (!file->m_Data)
				{
					fclose(filePointer);
					Log::Warning("Memory allocation failed.");
					return file;
				}

				int elementsRead = fread(file->m_Data, file->m_Size, 1, filePointer);
				if (elementsRead != 1)
				{
					fclose(filePointer);
					FreeMem(file->m_Data);
					file->m_Data = nullptr;
					Log::Warning("Failed to read file properly.");
					return file;
				}

				file->m_Data[file->m_Size] = '\0';
				fclose(filePointer);
			}
			else
			{
				file->m_Data = nullptr;
				file->m_Open = false;
				file->m_Size = 0;
			}

			return file;
		}

		void CloseFile(FileHandle* file)
		{
			if (file)
			{
				if (file->m_Data)
				{
					FreeMem(file->m_Data);
					file->m_Data = nullptr;
				}
				else
				{
					Log::Warning("Tried to free invalid file.");
				}
				FreeMem(file);
				file = nullptr;
			}
			else
			{
				Log::Warning("Tried to free invalid file.");
			}
		}

		bool WriteFile(const char* data, const CPath& filename)
		{
			std::ofstream outStream(filename.Path.c_str());
			outStream << data;
			outStream.close();
			return true;
		}

		bool CreateFile(const CPath& filename, const char* extToAppend)
		{
			CPath fileToWrite = filename;
			if (NCPath::FileExt(filename) == nullptr || NCPath::FileExt(filename)[0] == '\0')
			{
				fileToWrite = NCPath::CreatePath(filename.Path + std::string(extToAppend));
			}
			HANDLE fileHandle = CreateFileA(fileToWrite.Path.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			bool res = fileHandle != INVALID_HANDLE_VALUE;
			CloseHandle(fileHandle);
			return res;
		}

		bool DeleteFile(const CPath& filename)
		{
			return DeleteFileA(filename.Path.c_str());
		}

		bool CopyFile(const CPath& fileToCopy, const CPath& newFileLocation, const char* newFilename)
		{
			CPath newFilepath = newFileLocation;
			NCPath::Join(newFilepath, NCPath::CreatePath(std::string(newFilename) + NCPath::FileExt(fileToCopy)));
			if (!CopyFileExA(fileToCopy.Path.c_str(), newFilepath.Path.c_str(), NULL, NULL, false, NULL))
			{
				Log::Warning("Could not copy file error code: %d", GetLastError());
				return false;
			}

			return true;
		}

		CPath GetCwd()
		{
			char buff[FILENAME_MAX];
			char* success = _getcwd(buff, FILENAME_MAX);
			Log::Assert(success != NULL, "Unable to get Current Working Directory.");
			return { buff };
		}

		CPath GetSpecialAppFolder()
		{
			PWSTR pszPath;
			if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &pszPath)))
			{
				char* tmp = (char*)AllocMem(sizeof(char) * 256);
				wcstombs(tmp, pszPath, 256);
				CPath result = NCPath::CreatePath(tmp);
				FreeMem(tmp);
				return result;
			}

			Log::Assert(false, "Could not retrieve AppRoamingData folder.");
			return NCPath::CreatePath();
		}

		CPath GetExecutableDirectory()
		{
			char filepath[MAX_PATH];
			DWORD res = GetModuleFileNameA(NULL, filepath, MAX_PATH);
			Log::Assert(res != NULL && res != ERROR_INSUFFICIENT_BUFFER, "Get Executable Directory failed with error code: '%d'", res);

			return NCPath::CreatePath(filepath);
		}

		void CreateDirIfNotExists(const CPath& directory)
		{
			if (!(CreateDirectoryA(directory.Path.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()))
			{
				Log::Assert(false, "Failed to create directory %s", directory.Path.c_str());
			}
		}

		std::vector<CPath> GetFilesInDir(const CPath& directory)
		{
			std::vector<CPath> result;
			HANDLE dir;
			WIN32_FIND_DATAA fileData;

			CPath dirPath = directory;
			CPath wildcardMatch = dirPath;
			NCPath::Join(wildcardMatch, NCPath::CreatePath("*"));
			if ((dir = FindFirstFileA(wildcardMatch.Path.c_str(), &fileData)) == INVALID_HANDLE_VALUE)
			{
				// No file found
				return result;
			}

			do
			{
				CPath filename = NCPath::CreatePath(fileData.cFileName);
				CPath fullFilename = dirPath;
				NCPath::Join(fullFilename, filename);
				const bool isDirectory = (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);

				if (filename.Path[0] == '.')
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

		std::vector<CPath> GetFoldersInDir(const CPath& directory)
		{
			std::vector<CPath> result;
			HANDLE dir;
			WIN32_FIND_DATAA fileData;

			CPath dirPath = directory;
			CPath wildcardMatch = dirPath;
			NCPath::Join(wildcardMatch, NCPath::CreatePath("*"));
			if ((dir = FindFirstFileA(wildcardMatch.Path.c_str(), &fileData)) == INVALID_HANDLE_VALUE)
			{
				// No file found
				return result;
			}

			do
			{
				CPath filename = NCPath::CreatePath(fileData.cFileName);
				bool isDirectory = (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);

				if (filename.Path[0] == '.')
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

		CPath GetAbsolutePath(const CPath& path)
		{
			int bufferLength = GetFullPathNameA(path.Path.c_str(), 0, NULL, NULL);
			char* buffer = (char*)AllocMem(sizeof(char) * bufferLength);
			GetFullPathNameA(path.Path.c_str(), bufferLength, buffer, NULL);
			CPath result = NCPath::CreatePath(buffer);
			FreeMem(buffer);
			return result;
		}

		bool IsFile(const CPath& filepath)
		{
			DWORD fileAttributes = GetFileAttributesA(filepath.Path.c_str());
			return (fileAttributes != INVALID_FILE_ATTRIBUTES) && !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY);
		}

		bool IsHidden(const CPath& filepath)
		{
			DWORD fileAttributes = GetFileAttributesA(filepath.Path.c_str());
			return (fileAttributes != INVALID_FILE_ATTRIBUTES) && (fileAttributes & FILE_ATTRIBUTE_HIDDEN);
		}

		bool IsDirectory(const CPath& filepath)
		{
			DWORD fileAttributes = GetFileAttributesA(filepath.Path.c_str());
			return (fileAttributes != INVALID_FILE_ATTRIBUTES) && (fileAttributes & FILE_ATTRIBUTE_DIRECTORY);
		}

		bool RunProgram(const CPath& pathToExe, const char* cmdArguments)
		{
			STARTUPINFOA startupInfo;
			PROCESS_INFORMATION processInfo;

			ZeroMemory(&startupInfo, sizeof(startupInfo));
			startupInfo.cb = sizeof(startupInfo);
			ZeroMemory(&processInfo, sizeof(processInfo));

			std::string cmdArgs = pathToExe.Path.c_str() + std::string(" ") + cmdArguments;

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
				Log::Warning("Unsuccessfully started process '%s'", pathToExe.Path.c_str());
			}

			return res;
		}

		bool RunProgram(const CPath& pathToExe, const std::string& cmdArgs)
		{
			return RunProgram(pathToExe, cmdArgs.c_str());
		}
	}
}
#endif