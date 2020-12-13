#include "jade/file/IFile.h"
#include "jade/util/Log.h"

#include <direct.h>
#include <shobjidl_core.h>
#include <shlobj.h>
#include <knownfolders.h>

namespace Jade
{
	File* Win32File::ImplOpenFile(const JPath& filename)
	{
		File* file = new File();
		file->m_Filename = filename.Filepath();

		std::ifstream inputStream(filename.Filepath());

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

	bool Win32File::ImplWriteFile(const char* data, const JPath& filename)
	{
		std::ofstream outStream(filename.Filepath());
		outStream << data;
		outStream.close();
		return true;
	}

	bool Win32File::ImplCreateFile(const JPath& filename, const char* extToAppend)
	{
		JPath fileToWrite = filename;
		if (filename.FileExt() == nullptr || filename.FileExt()[0] == '\0')
		{
			fileToWrite = JPath(filename.Filepath() + std::string(extToAppend));
		}
		HANDLE fileHandle = CreateFileA(fileToWrite.Filepath(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		bool res = fileHandle != INVALID_HANDLE_VALUE;
		CloseHandle(fileHandle);
		return res;
	}

	bool Win32File::ImplCopyFile(const JPath& fileToCopy, const JPath& newFileLocation, const char* newFilename)
	{
		JPath newFilepath = newFileLocation + (std::string(newFilename) + fileToCopy.FileExt());
		return CopyFileExA(fileToCopy.Filepath(), newFilepath.Filepath(), NULL, NULL, false, NULL);
	}

	JPath Win32File::ImplGetCwd()
	{
		char buff[FILENAME_MAX];
		char* success = _getcwd(buff, FILENAME_MAX);
		Log::Assert(success != NULL, "Unable to get Current Working Directory.");
		return { buff };
	}

	JPath Win32File::ImplGetSpecialAppFolder()
	{
		PWSTR pszPath;
		if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &pszPath)))
		{
			char* tmp = new char[256];
			wcstombs(tmp, pszPath, 256);
			JPath result(tmp);
			delete[] tmp;
			return result;
		}

		Log::Assert(false, "Could not retrieve AppRoamingData folder.");
		return JPath("");
	}

	JPath Win32File::ImplGetExecutableDirectory()
	{
		char filepath[MAX_PATH];
		DWORD res = GetModuleFileNameA(NULL, filepath, MAX_PATH);
		Log::Assert(res != NULL && res != ERROR_INSUFFICIENT_BUFFER, "Get Executable Directory failed with error code: '%d'", res);

		return JPath(filepath);
	}

	void Win32File::ImplCreateDirIfNotExists(const JPath& directory)
	{
		if (!(CreateDirectoryA(directory.Filepath(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()))
		{
			Log::Assert(false, "Failed to create directory %s", directory);
		}
	}

	std::vector<JPath> Win32File::ImplGetFilesInDir(const JPath& directory)
	{
		std::vector<JPath> result;
		HANDLE dir;
		WIN32_FIND_DATAA fileData;

		JPath dirString{ directory };
		if ((dir = FindFirstFileA((dirString + JPath("*")).Filepath(), &fileData)) == INVALID_HANDLE_VALUE)
		{
			// No file found
			return result;
		}

		do
		{
			const JPath filename{ fileData.cFileName };
			const JPath fullFilename = dirString + filename;
			const bool isDirectory = (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);

			if (filename[0] == '.')
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

	std::vector<JPath> Win32File::ImplGetFoldersInDir(const JPath& directory)
	{
		std::vector<JPath> result;
		HANDLE dir;
		WIN32_FIND_DATAA fileData;

		JPath dirString = JPath(directory);
		if ((dir = FindFirstFileA((dirString + JPath("*")).Filepath(), &fileData)) == INVALID_HANDLE_VALUE)
		{
			// No file found
			return result;
		}

		do
		{
			const JPath filename = fileData.cFileName;
			const bool isDirectory = (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);

			if (filename[0] == '.')
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

	JPath Win32File::ImplGetAbsolutePath(const JPath& path)
	{
		int bufferLength = GetFullPathNameA(path.Filepath(), 0, NULL, NULL);
		char* buffer = new char[bufferLength];
		GetFullPathNameA(path.Filepath(), bufferLength, buffer, NULL);
		JPath result = JPath(buffer);
		delete[] buffer;
		return result;
	}

	bool Win32File::ImplIsFile(const JPath& filepath)
	{
		return !IsDirectory(filepath);
	}

	bool Win32File::ImplIsDirectory(const JPath& filepath)
	{
		return (GetFileAttributesA(filepath.Filepath()) & FILE_ATTRIBUTE_DIRECTORY);
	}

	bool Win32File::ImplRunProgram(const JPath& pathToExe, const char* cmdArguments)
	{
		STARTUPINFOA startupInfo;
		PROCESS_INFORMATION processInfo;

		ZeroMemory(&startupInfo, sizeof(startupInfo));
		startupInfo.cb = sizeof(startupInfo);
		ZeroMemory(&processInfo, sizeof(processInfo));

		// Start the program
		bool res = CreateProcessA(
			pathToExe.Filepath(),  // Application Name
			(LPSTR)cmdArguments,   // Command Line Args
			NULL,                  // Process Attributes
			NULL,                  // Thread Attributes
			FALSE,                 // Inherit Handles
			DETACHED_PROCESS,      // Creation Flags
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
			Log::Warning("Unsuccessfully started process '%s'", pathToExe.Filepath());
		}

		return res;
	}
}