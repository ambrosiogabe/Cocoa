#pragma once
#include "externalLibs.h"
#include "jade/file/JPath.h"

namespace Jade
{
	struct File
	{
		const char* m_Filename = nullptr;
		std::string m_Data = "";
		uint32 m_Size = 0;
		bool m_Open = false;
	};

	class JADE IFile
	{
	public:
		static void Init();
		static void Destroy();

		static File* OpenFile(const JPath& filename) { return Get()->ImplOpenFile(filename); }
		static void CloseFile(File* file) { Get()->ImplCloseFile(file); }
		static bool WriteFile(const char* data, const JPath& filename) { return Get()->ImplWriteFile(data, filename); }
		static bool CreateFile(const JPath& filename, const char* extToAppend = "") { return Get()->ImplCreateFile(filename, extToAppend); }
		static bool CopyFile(const JPath& fileToCopy, const JPath& newFileLocation, const char* newFilename = "") { return Get()->ImplCopyFile(fileToCopy, newFileLocation, newFilename); }
		static JPath GetCwd() { return Get()->ImplGetCwd(); }
		static JPath GetSpecialAppFolder() { return Get()->ImplGetSpecialAppFolder(); }
		static JPath GetExecutableDirectory() { return Get()->ImplGetExecutableDirectory(); }
		static std::vector<JPath> GetFilesInDir(const JPath& directory) { return Get()->ImplGetFilesInDir(directory); }
		static std::vector<JPath> GetFoldersInDir(const JPath& directory) { return Get()->ImplGetFoldersInDir(directory); }
		static void CreateDirIfNotExists(const JPath& directory) { Get()->ImplCreateDirIfNotExists(directory); }
		static bool IsFile(const JPath& filepath) { return Get()->ImplIsFile(filepath); }
		static bool IsDirectory(const JPath& directory) { return Get()->ImplIsDirectory(directory); }
		static JPath GetAbsolutePath(const JPath& path) { return Get()->ImplGetAbsolutePath(path); }

		static bool RunProgram(const JPath& pathToExe, const char* cmdArgs = "") { return Get()->ImplRunProgram(pathToExe, cmdArgs); }
		static bool RunProgram(const JPath& pathToExe, const std::string& cmdArgs = "") { return Get()->ImplRunProgram(pathToExe, cmdArgs.c_str()); }

	protected:
		virtual File* ImplOpenFile(const JPath& filename) = 0;
		virtual void ImplCloseFile(File* file) = 0;
		virtual bool ImplWriteFile(const char* data, const JPath& filename) = 0;
		virtual bool ImplCreateFile(const JPath& filename, const char* extToAppend) = 0;
		virtual bool ImplCopyFile(const JPath& fileToCopy, const JPath& newFileLocation, const char* newFilename) = 0;
		virtual JPath ImplGetCwd() = 0;
		virtual JPath ImplGetSpecialAppFolder() = 0;
		virtual JPath ImplGetExecutableDirectory() = 0;
		virtual std::vector<JPath> ImplGetFilesInDir(const JPath& directory) = 0;
		virtual std::vector<JPath> ImplGetFoldersInDir(const JPath& directory) = 0;
		virtual void ImplCreateDirIfNotExists(const JPath& directory) = 0;
		virtual bool ImplIsFile(const JPath& filepath) = 0;
		virtual bool ImplIsDirectory(const JPath& filepath) = 0;
		virtual JPath ImplGetAbsolutePath(const JPath& path) = 0;
		
		virtual bool ImplRunProgram(const JPath& pathToExe, const char* cmdArgs) = 0;

	private:
		static IFile* Get();

	private:
		static IFile* s_Instance;
	};

#ifdef _WIN32
	class JADE Win32File : public IFile
	{
	public:
		static IFile* Create()
		{
			return new Win32File();
		}

		virtual File* ImplOpenFile(const JPath& filename) override;
		virtual void ImplCloseFile(File* file) override;
		virtual bool ImplWriteFile(const char* data, const JPath& filename) override;
		virtual bool ImplCreateFile(const JPath& filename, const char* extToAppend) override;
		virtual bool ImplCopyFile(const JPath& fileToCopy, const JPath& newFileLocation, const char* newFilename) override;
		virtual JPath ImplGetCwd() override;
		virtual JPath ImplGetSpecialAppFolder() override;
		virtual JPath ImplGetExecutableDirectory() override;
		virtual std::vector<JPath> ImplGetFilesInDir(const JPath& directory) override;
		virtual std::vector<JPath> ImplGetFoldersInDir(const JPath& directory) override;
		virtual void ImplCreateDirIfNotExists(const JPath& directory) override;
		virtual bool ImplIsFile(const JPath& filepath) override;
		virtual bool ImplIsDirectory(const JPath& filepath) override;
		virtual JPath ImplGetAbsolutePath(const JPath& path) override;

		virtual bool ImplRunProgram(const JPath& pathToExe, const char* cmdArgs) override;
	};
#endif
}