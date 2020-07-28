#pragma once
#include "externalLibs.h"
#include "jade/file/IFile.h"

namespace Jade
{
	class JPath
	{
	public:
		JPath(std::string path);
		JPath(const JPath& other);
		~JPath();

		inline const char* Filename() { return m_Filename; }
		inline const char* Filepath() { return m_Filepath; }
		inline const char* FileExt()  { return m_FileExt; }
		inline bool IsFile() { return IFile::IsFile(m_Filepath); }
		inline bool IsDirectory() { return IFile::IsDirectory(m_Filepath); }
		std::string GetDirectory(int level);

		void Join(const JPath& other);

		JPath operator +(std::string other);
		JPath operator +(const JPath& other);

	protected:
		static char PATH_SEPARATOR;

	private:
		inline bool IsSeparator(char c)
		{
			return (c == WIN_SEPARTOR || c == UNIX_SEPARATOR);
		}

	private:
		static const char WIN_SEPARTOR = '\\';
		static const char UNIX_SEPARATOR = '/';

		int m_PathLength;
		char* m_Filepath;
		char* m_Filename;
		char* m_FileExt;
	};

#ifdef _WIN32
	class Win32JPath
	{
	public:
		static const char GetPathSeparator()
		{
			return '\\';
		}
	};
#endif
}