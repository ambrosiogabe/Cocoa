#include "cocoa/file/IFile.h"
#include "cocoa/util/Log.h"

#include <direct.h>
#include <shobjidl_core.h>
#include <shlobj.h>
#include <knownfolders.h>

namespace Cocoa
{
	File* Win32File::ImplOpenFile(const CPath& filename)
	{
		File* file = new File();
		file->m_Filename = filename.Path.c_str();

		std::ifstream inputStream(filename.Path.c_str());

		if (inputStream.is_open())
		{
			inputStream.seekg(0, std::ios::end);
			file->m_Size = (uint32)inputStream.tellg();
			file->m_Data.reserve(file->m_Size);
			inputStream.seekg(0, std::ios::beg);

			file->m_Data.assign((std::istreambuf_iterator<char>(inputStream)), std::istreambuf_iterator<char>());
		}
		else
		{
			file->m_Data = "";
			file->m_Open = false;
			file->m_Size = 0;
		}

		return file;
	}

	void Win32File::ImplCloseFile(File* file)
	{
		delete file;
	}

	bool Win32File::ImplWriteFile(const char* data, const CPath& filename)
	{
		std::ofstream outStream(filename.Path.c_str());
		outStream << data;
		outStream.close();
		return true;
	}

	bool Win32File::ImplCreateFile(const CPath& filename, const char* extToAppend)
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

	bool Win32File::ImplDeleteFile(const CPath& filename)
	{
		return DeleteFileA(filename.Path.c_str());
	}

	bool Win32File::ImplCopyFile(const CPath& fileToCopy, const CPath& newFileLocation, const char* newFilename)
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

	CPath Win32File::ImplGetCwd()
	{
		char buff[FILENAME_MAX];
		char* success = _getcwd(buff, FILENAME_MAX);
		Log::Assert(success != NULL, "Unable to get Current Working Directory.");
		return { buff };
	}

	CPath Win32File::ImplGetSpecialAppFolder()
	{
		PWSTR pszPath;
		if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &pszPath)))
		{
			char* tmp = new char[256];
			wcstombs(tmp, pszPath, 256);
			CPath result = NCPath::CreatePath(tmp);
			delete[] tmp;
			return result;
		}

		Log::Assert(false, "Could not retrieve AppRoamingData folder.");
		return NCPath::CreatePath();
	}

	CPath Win32File::ImplGetExecutableDirectory()
	{
		char filepath[MAX_PATH];
		DWORD res = GetModuleFileNameA(NULL, filepath, MAX_PATH);
		Log::Assert(res != NULL && res != ERROR_INSUFFICIENT_BUFFER, "Get Executable Directory failed with error code: '%d'", res);

		return NCPath::CreatePath(filepath);
	}

	void Win32File::ImplCreateDirIfNotExists(const CPath& directory)
	{
		if (!(CreateDirectoryA(directory.Path.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()))
		{
			Log::Assert(false, "Failed to create directory %s", directory.Path.c_str());
		}
	}

	std::vector<CPath> Win32File::ImplGetFilesInDir(const CPath& directory)
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

	std::vector<CPath> Win32File::ImplGetFoldersInDir(const CPath& directory)
	{
		std::vector<CPath> result;
		HANDLE dir;
		WIN32_FIND_DATAA fileData;

		CPath dirPath = directory;
		CPath wildcardMatch = dirPath;
		NCPath::Join(wildcardMatch, NCPath::CreatePath("*"));
		if ((dir = FindFirstFileA(dirPath.Path.c_str(), &fileData)) == INVALID_HANDLE_VALUE)
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

	CPath Win32File::ImplGetAbsolutePath(const CPath& path)
	{
		int bufferLength = GetFullPathNameA(path.Path.c_str(), 0, NULL, NULL);
		char* buffer = new char[bufferLength];
		GetFullPathNameA(path.Path.c_str(), bufferLength, buffer, NULL);
		CPath result = NCPath::CreatePath(buffer);
		delete[] buffer;
		return result;
	}

	bool Win32File::ImplIsFile(const CPath& filepath)
	{
		DWORD fileAttributes = GetFileAttributesA(filepath.Path.c_str());
		return (fileAttributes != INVALID_FILE_ATTRIBUTES) && !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY);
	}

	bool Win32File::ImplIsHidden(const CPath& filepath)
	{
		DWORD fileAttributes = GetFileAttributesA(filepath.Path.c_str());
		return (fileAttributes != INVALID_FILE_ATTRIBUTES) && (fileAttributes & FILE_ATTRIBUTE_HIDDEN);
	}

	bool Win32File::ImplIsDirectory(const CPath& filepath)
	{
		DWORD fileAttributes = GetFileAttributesA(filepath.Path.c_str());
		return (fileAttributes != INVALID_FILE_ATTRIBUTES) && (fileAttributes & FILE_ATTRIBUTE_DIRECTORY);
	}

	bool Win32File::ImplRunProgram(const CPath& pathToExe, const char* cmdArguments)
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
}