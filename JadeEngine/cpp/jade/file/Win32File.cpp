#include "jade/file/IFile.h"
#include "jade/util/Log.h"

#include <direct.h>
#include <shobjidl_core.h>
#include <shlobj.h>
#include <knownfolders.h>

namespace Jade
{
	File* Win32File::ImplOpenFile(const char* filename)
	{
		File* file = new File();
		file->m_Filename = filename;

		std::ifstream inputStream(filename);

		inputStream.seekg(0, std::ios::end);
		file->m_Size = inputStream.tellg();
		file->m_Data.reserve(file->m_Size);
		inputStream.seekg(0, std::ios::beg);

		file->m_Data.assign((std::istreambuf_iterator<char>(inputStream)), std::istreambuf_iterator<char>());

		return file;
	}

	void Win32File::ImplCloseFile(File* file)
	{
		delete file;
	}

	bool Win32File::ImplWriteFile(const char* data, const char* filename)
	{
		std::ofstream outStream(filename);
		outStream << data;
		outStream.close();
		return true;
	}

	std::string Win32File::ImplGetCwd()
	{
		char buff[FILENAME_MAX];
		_getcwd(buff, FILENAME_MAX);
		return std::string(buff);
	}

	std::string Win32File::ImplGetSpecialAppFolder()
	{
		PWSTR pszPath;
		if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &pszPath)))
		{
			char* tmp = new char[256];
			wcstombs(tmp, pszPath, 256);
			std::string result = std::string(tmp);
			delete tmp;
			return result;
		}

		Log::Assert(false, "Could not retrieve AppRoamingData folder.");
		return std::string();
	}

	void Win32File::ImplCreateDirIfNotExists(const char* directory)
	{
		if (!(CreateDirectoryA(directory, NULL) || ERROR_ALREADY_EXISTS == GetLastError()))
		{
			Log::Assert(false, "Failed to create directory %s", directory);
		}
	}

	std::vector<std::string> Win32File::ImplGetFilesInDir(const char* directory)
	{
		std::vector<std::string> result;
		HANDLE dir;
		WIN32_FIND_DATAA fileData;

		std::string dirString = std::string(directory);
		if ((dir = FindFirstFileA((dirString + "\\*").c_str(), &fileData)) == INVALID_HANDLE_VALUE)
		{
			// No file found
			return result;
		}

		do
		{
			const std::string filename = fileData.cFileName;
			const std::string fullFilename = dirString + "\\" + filename;
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

	std::vector<std::string> Win32File::ImplGetFoldersInDir(const char* directory)
	{
		std::vector<std::string> result;
		HANDLE dir;
		WIN32_FIND_DATAA fileData;

		std::string dirString = std::string(directory);
		if ((dir = FindFirstFileA((dirString + "\\*").c_str(), &fileData)) == INVALID_HANDLE_VALUE)
		{
			// No file found
			return result;
		}

		do
		{
			const std::string filename = fileData.cFileName;
			const std::string fullFilename = dirString + "\\" + filename;
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
}