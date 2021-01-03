#pragma once
#include "externalLibs.h"
#include "cocoa/file/CPath.h"

// TODO: Windows file leaking somewhere, figure out where so I don't have to do this...
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
	struct File
	{
		const char* m_Filename = nullptr;
		std::string m_Data = "";
		uint32 m_Size = 0;
		bool m_Open = false;
	};

	class COCOA IFile
	{
	public:
		static void Init();
		static void Destroy();

		static File* OpenFile(const CPath& filename) { return Get()->ImplOpenFile(filename); }
		static void CloseFile(File* file) { Get()->ImplCloseFile(file); }
		static bool WriteFile(const char* data, const CPath& filename) { return Get()->ImplWriteFile(data, filename); }
		static bool CreateFile(const CPath& filename, const char* extToAppend = "") { return Get()->ImplCreateFile(filename, extToAppend); }
		static bool DeleteFile(const CPath& filename) { return Get()->ImplDeleteFile(filename); }
		static bool CopyFile(const CPath& fileToCopy, const CPath& newFileLocation, const char* newFilename = "") { return Get()->ImplCopyFile(fileToCopy, newFileLocation, newFilename); }
		static CPath GetCwd() { return Get()->ImplGetCwd(); }
		static CPath GetSpecialAppFolder() { return Get()->ImplGetSpecialAppFolder(); }
		static CPath GetExecutableDirectory() { return Get()->ImplGetExecutableDirectory(); }
		static std::vector<CPath> GetFilesInDir(const CPath& directory) { return Get()->ImplGetFilesInDir(directory); }
		static std::vector<CPath> GetFoldersInDir(const CPath& directory) { return Get()->ImplGetFoldersInDir(directory); }
		static void CreateDirIfNotExists(const CPath& directory) { Get()->ImplCreateDirIfNotExists(directory); }
		static bool IsFile(const CPath& filepath) { return Get()->ImplIsFile(filepath); }
		static bool IsHidden(const CPath& filepath) { return Get()->ImplIsHidden(filepath); }
		static bool IsDirectory(const CPath& directory) { return Get()->ImplIsDirectory(directory); }
		static CPath GetAbsolutePath(const CPath& path) { return Get()->ImplGetAbsolutePath(path); }

		static bool RunProgram(const CPath& pathToExe, const char* cmdArgs = "") { return Get()->ImplRunProgram(pathToExe, cmdArgs); }
		static bool RunProgram(const CPath& pathToExe, const std::string& cmdArgs = "") { return Get()->ImplRunProgram(pathToExe, cmdArgs.c_str()); }

	protected:
		virtual File* ImplOpenFile(const CPath& filename) = 0;
		virtual void ImplCloseFile(File* file) = 0;
		virtual bool ImplWriteFile(const char* data, const CPath& filename) = 0;
		virtual bool ImplCreateFile(const CPath& filename, const char* extToAppend) = 0;
		virtual bool ImplDeleteFile(const CPath& filename) = 0;
		virtual bool ImplCopyFile(const CPath& fileToCopy, const CPath& newFileLocation, const char* newFilename) = 0;
		virtual CPath ImplGetCwd() = 0;
		virtual CPath ImplGetSpecialAppFolder() = 0;
		virtual CPath ImplGetExecutableDirectory() = 0;
		virtual std::vector<CPath> ImplGetFilesInDir(const CPath& directory) = 0;
		virtual std::vector<CPath> ImplGetFoldersInDir(const CPath& directory) = 0;
		virtual void ImplCreateDirIfNotExists(const CPath& directory) = 0;
		virtual bool ImplIsFile(const CPath& filepath) = 0;
		virtual bool ImplIsHidden(const CPath& filepath) = 0;
		virtual bool ImplIsDirectory(const CPath& filepath) = 0;
		virtual CPath ImplGetAbsolutePath(const CPath& path) = 0;
		
		virtual bool ImplRunProgram(const CPath& pathToExe, const char* cmdArgs) = 0;

	private:
		static IFile* Get();

	private:
		static IFile* s_Instance;
	};

#ifdef _WIN32
	class COCOA Win32File : public IFile
	{
	public:
		static IFile* Create()
		{
			return new Win32File();
		}

		virtual File* ImplOpenFile(const CPath& filename) override;
		virtual void ImplCloseFile(File* file) override;
		virtual bool ImplWriteFile(const char* data, const CPath& filename) override;
		virtual bool ImplCreateFile(const CPath& filename, const char* extToAppend) override;
		virtual bool ImplDeleteFile(const CPath& filename) override;
		virtual bool ImplCopyFile(const CPath& fileToCopy, const CPath& newFileLocation, const char* newFilename) override;
		virtual CPath ImplGetCwd() override;
		virtual CPath ImplGetSpecialAppFolder() override;
		virtual CPath ImplGetExecutableDirectory() override;
		virtual std::vector<CPath> ImplGetFilesInDir(const CPath& directory) override;
		virtual std::vector<CPath> ImplGetFoldersInDir(const CPath& directory) override;
		virtual void ImplCreateDirIfNotExists(const CPath& directory) override;
		virtual bool ImplIsFile(const CPath& filepath) override;
		virtual bool ImplIsHidden(const CPath& filepath) override;
		virtual bool ImplIsDirectory(const CPath& filepath) override;
		virtual CPath ImplGetAbsolutePath(const CPath& path) override;

		virtual bool ImplRunProgram(const CPath& pathToExe, const char* cmdArgs) override;
	};
#endif
}