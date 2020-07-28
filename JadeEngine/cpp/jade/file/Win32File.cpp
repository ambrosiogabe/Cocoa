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

	bool Win32File::ImplWriteFile(const char* data, const JPath& filename)
	{
		std::ofstream outStream(filename.Filepath());
		outStream << data;
		outStream.close();
		return true;
	}

	JPath Win32File::ImplGetCwd()
	{
		char buff[FILENAME_MAX];
		_getcwd(buff, FILENAME_MAX);
		return {buff};
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

		JPath dirString{directory};
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
			const JPath fullFilename = dirString + filename;
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
		char* buffer = new char[MAX_PATH];
		GetFullPathNameA(path.Filepath(), MAX_PATH, buffer, NULL);
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
}