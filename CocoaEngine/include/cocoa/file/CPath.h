#pragma once
#include "externalLibs.h"

namespace Cocoa
{
	class COCOA CPath
	{
	public:
		CPath(bool forceLinuxStylePath=false)
		{
			m_Filepath = "";
			m_FilenameOffset = 0;
			m_FileExtOffset = 0;
		}

		CPath(std::string path, bool forceLinuxStylePath=false);
		CPath(const char* path, bool forceLinuxStylePath=false);
		CPath(const CPath& other);
		CPath(CPath&& other) noexcept;
		~CPath();
		CPath& operator=(CPath& other);
		CPath& operator=(CPath&& other) noexcept;
		CPath& operator=(const CPath& other);

		inline int FilenameSize() const { return (int)(m_Filepath.size() - m_FilenameOffset); }
		inline int FileExtSize() const { return (int)(m_Filepath.size() - m_FileExtOffset); }
		inline int Size() const { return (int)m_Filepath.size(); }
		inline const char* Filename() const { return &m_Filepath[m_FilenameOffset]; }
		inline const char* Filepath() const { return m_Filepath.c_str(); }
		inline const char* FileExt()  const { return &m_Filepath[m_FileExtOffset]; }
		inline bool IsFile() const;
		inline bool IsDirectory();
		std::string GetDirectory(int level) const;
		std::string GetFilenameWithoutExt() const;

		void Join(const CPath& other);
		bool Contains(const char* pathSegment) const;

		CPath operator+(std::string other) const;
		CPath operator+(const CPath& other) const;
		CPath operator+(const char* other) const;
		bool operator==(const CPath& other) const;

		void operator+=(const CPath& other);
		char operator[](int index) const;

	protected:
		static char PATH_SEPARATOR;

	private:
		inline bool IsSeparator(char c) const
		{
			return (c == WIN_SEPARTOR || c == UNIX_SEPARATOR);
		}

		void Init(const char* path, int pathSize);

	private:
		static const char WIN_SEPARTOR = '\\';
		static const char UNIX_SEPARATOR = '/';

		char m_PathSeparator;
		std::string m_Filepath;
		int m_FilenameOffset;
		int m_FileExtOffset;
	};

#ifdef _WIN32
	class COCOA Win32CPath
	{
	public:
		static const char GetPathSeparator()
		{
			return '\\';
		}
	};
#endif
}