#pragma once
#include "externalLibs.h"

namespace Jade
{
	class JADE JPath
	{
	public:
		JPath(std::string path);
		JPath(const char* path);
		JPath(const JPath& other);
		JPath(JPath&& other) noexcept;
		~JPath();
		JPath& operator=(JPath& other);
		JPath& operator=(JPath&& other) noexcept;
		JPath& operator=(const JPath& other);

		inline int FilenameSize() const { return (int)(m_Filepath.size() - m_FilenameOffset); }
		inline int Size() const { return (int)m_Filepath.size(); }
		inline const char* Filename() const { return &m_Filepath[m_FilenameOffset]; }
		inline const char* Filepath() const { return m_Filepath.c_str(); }
		inline const char* FileExt()  const { return &m_Filepath[m_FileExtOffset]; }
		inline bool IsFile();
		inline bool IsDirectory();
		std::string GetDirectory(int level);

		void Join(const JPath& other);

		JPath operator+(std::string other) const;
		JPath operator+(const JPath& other) const;
		JPath operator+(const char* other) const;
		bool operator==(const JPath& other) const;

		void operator+=(const JPath& other);
		char operator[](int index) const;

	protected:
		static char PATH_SEPARATOR;

	private:
		inline bool IsSeparator(char c)
		{
			return (c == WIN_SEPARTOR || c == UNIX_SEPARATOR);
		}

		void Init(const char* path, int pathSize);

	private:
		static const char WIN_SEPARTOR = '\\';
		static const char UNIX_SEPARATOR = '/';

		std::string m_Filepath;
		int m_FilenameOffset;
		int m_FileExtOffset;
	};

#ifdef _WIN32
	class JADE Win32JPath
	{
	public:
		static const char GetPathSeparator()
		{
			return '\\';
		}
	};
#endif
}