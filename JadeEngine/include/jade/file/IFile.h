#pragma once
#include "externalLibs.h"

namespace Jade
{
	struct File
	{
		const char* m_Filename;
		std::string m_Data;
		uint32 m_Size;
		bool m_Open;
	};

	class IFile
	{
	public:
		static void Init();
		static void Destroy();

		static File* OpenFile(const char* filename) { return Get()->ImplOpenFile(filename); }
		static void CloseFile(File* file) { Get()->ImplCloseFile(file); }
		static bool WriteFile(const char* data, const char* filename) { return Get()->ImplWriteFile(data, filename); }
		static std::string GetCwd() { return Get()->ImplGetCwd(); }
		static std::string GetSpecialAppFolder() { return Get()->ImplGetSpecialAppFolder(); }
		static std::vector<std::string> GetFilesInDir(const char* directory) { return Get()->ImplGetFilesInDir(directory); }
		static std::vector<std::string> GetFoldersInDir(const char* directory) { return Get()->ImplGetFoldersInDir(directory); }
		static void CreateDirIfNotExists(const char* directory) { Get()->ImplCreateDirIfNotExists(directory); }

	protected:
		virtual File* ImplOpenFile(const char* filename) = 0;
		virtual void ImplCloseFile(File* file) = 0;
		virtual bool ImplWriteFile(const char* data, const char* filename) = 0;
		virtual std::string ImplGetCwd() = 0;
		virtual std::string ImplGetSpecialAppFolder() = 0;
		virtual std::vector<std::string> ImplGetFilesInDir(const char* directory) = 0;
		virtual std::vector<std::string> ImplGetFoldersInDir(const char* directory) = 0;
		virtual void ImplCreateDirIfNotExists(const char* directory) = 0;

	private:
		static IFile* Get();

	private:
		static IFile* s_Instance;
	};

#ifdef _WIN32
	class Win32File : public IFile
	{
	public:
		static IFile* Create()
		{
			return new Win32File();
		}

		virtual File* ImplOpenFile(const char* filename) override;
		virtual void ImplCloseFile(File* file) override;
		virtual bool ImplWriteFile(const char* data, const char* filename) override;
		virtual std::string ImplGetCwd() override;
		virtual std::string ImplGetSpecialAppFolder() override;
		virtual std::vector<std::string> ImplGetFilesInDir(const char* directory) override;
		virtual std::vector<std::string> ImplGetFoldersInDir(const char* directory) override;
		virtual void ImplCreateDirIfNotExists(const char* directory) override;
	};
#endif
}