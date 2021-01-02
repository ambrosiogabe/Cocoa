#include "cocoa/file/CPath.h"
#include "cocoa/util/Log.h"
#include "cocoa/file/IFile.h"

namespace Cocoa
{
#ifdef _WIN32
	char CPath::PATH_SEPARATOR = Win32CPath::GetPathSeparator();
#endif

	CPath::CPath(std::string path, bool forceLinuxStylePath)
	{
		m_PathSeparator = CPath::PATH_SEPARATOR;
		if (forceLinuxStylePath)
		{
			m_PathSeparator = '/';
		}

		Init(path.c_str(), (int)path.size());
	}

	CPath::CPath(const char* path, bool forceLinuxStylePath)
	{
		m_PathSeparator = CPath::PATH_SEPARATOR;
		if (forceLinuxStylePath)
		{
			m_PathSeparator = '/';
		}

		int pathSize = 0;
		if (path != nullptr)
		{
			for (const char* iter = path; *iter != '\0'; iter++)
			{
				pathSize++;
			}
		}

		Init(path, pathSize);
	}

	CPath::CPath(const CPath& other)
	{
		m_PathSeparator = other.m_PathSeparator;
		m_Filepath = other.m_Filepath;
		m_FilenameOffset = other.m_FilenameOffset;
		m_FileExtOffset = other.m_FileExtOffset;
	}

	void CPath::Init(const char* path, int pathSize)
	{
		if (pathSize == 0)
		{
			m_Filepath = "";
			m_FileExtOffset = 0;
			m_FilenameOffset = 0;
			return;
		}

		char* sanitizedPath = new char[pathSize];
		int lastPathSeparator = -1;
		int stringIndex = 0;
		int pathIndex = 0;
		int lastDot = -1;

		const char* iterEnd = path + pathSize;
		for (const char* iter = path; iter != iterEnd; iter++)
		{
			char c = *iter;
			if (c == '.' && stringIndex > 0 && sanitizedPath[stringIndex - 1] != '.')
			{
				lastDot = pathIndex;
			}
			else if (c == '.' && stringIndex > 0 && sanitizedPath[stringIndex - 1] == '.')
			{
				// We have a double dot ..
				// If .. is at the end of the path or .. is followed by a path separator '/'
				if (stringIndex == pathSize - 1 || (stringIndex + 1 < pathSize && IsSeparator(path[stringIndex + 1])))
				{
					lastDot = -1;
				}
				else
				{
					lastDot = pathIndex;
				}
			}

			if (IsSeparator(c) && (stringIndex == 0 || stringIndex == 1 || !IsSeparator(sanitizedPath[stringIndex - 1])))
			{
				sanitizedPath[pathIndex] = m_PathSeparator;
				lastPathSeparator = pathIndex;
				pathIndex++;
			}
			else if (!IsSeparator(c))
			{
				sanitizedPath[pathIndex] = c;
				pathIndex++;
			}

			stringIndex++;
		}

		m_Filepath = std::string(sanitizedPath, pathIndex);
		m_FilenameOffset = lastPathSeparator >= -1 && lastPathSeparator != pathIndex
			? lastPathSeparator + 1 : pathIndex;
		m_FileExtOffset = (lastDot == -1 || lastDot < lastPathSeparator || lastPathSeparator == lastDot - 1)
			? pathIndex : lastDot;

		delete[] sanitizedPath;
	}

	CPath::CPath(CPath&& other) noexcept
		: m_Filepath(std::move(other.m_Filepath))
	{
		m_PathSeparator = other.m_PathSeparator;
		m_FilenameOffset = other.m_FilenameOffset;
		m_FileExtOffset = other.m_FileExtOffset;
	}

	CPath& CPath::operator=(CPath&& other) noexcept
	{
		if (this != &other)
		{
			// Free the existing resource

			// Copy the data pointers from other to here
			m_PathSeparator = other.m_PathSeparator;
			m_Filepath = std::move(other.m_Filepath);
			m_FileExtOffset = other.m_FileExtOffset;
			m_FilenameOffset = other.m_FilenameOffset;

			// Release the data pointers from source, so destructor does not free multiple times
		}

		return *this;
	}

	CPath& CPath::operator=(const CPath& other)
	{
		if (&other != this)
		{
			m_PathSeparator = other.m_PathSeparator;
			m_Filepath = other.m_Filepath;
			m_FilenameOffset = other.m_FilenameOffset;
			m_FileExtOffset = other.m_FileExtOffset;
		}

		return *this;
	}

	CPath& CPath::operator=(CPath& other)
	{
		if (&other != this)
		{
			m_PathSeparator = other.m_PathSeparator;
			m_Filepath = other.m_Filepath;
			m_FilenameOffset = other.m_FilenameOffset;
			m_FileExtOffset = other.m_FileExtOffset;
		}

		return *this;
	}

	CPath::~CPath()
	{
	}

	CPath CPath::operator+(std::string other) const
	{
		CPath copy = CPath(*this);
		copy.Join(CPath(other));
		return copy;
	}

	CPath CPath::operator+(const char* other) const
	{
		CPath copy = CPath(*this);
		copy.Join(CPath(other));
		return copy;
	}

	CPath CPath::operator+(const CPath& other) const
	{
		CPath copy = CPath(*this);
		copy.Join(other);
		return copy;
	}

	void CPath::operator+=(const CPath& other)
	{
		this->Join(other);
	}

	bool CPath::operator==(const CPath& other) const
	{
		if (m_Filepath.size() == 0 || other.m_Filepath.size() == 0)
		{
			return false;
		}

		CPath tmp1 = IFile::GetAbsolutePath(other);
		CPath tmp2 = IFile::GetAbsolutePath(*this);
		return strcmp(tmp1.m_Filepath.c_str(), tmp2.m_Filepath.c_str()) == 0;
	}

	char CPath::operator[](int index) const
	{
		return m_Filepath[index];
	}

	bool CPath::Contains(const char* pathSegment) const
	{
		return m_Filepath.find(pathSegment) != std::string::npos;
	}

	void CPath::Join(const CPath& other)
	{
		int newLength = (int)(other.m_Filepath.size() + m_Filepath.size());
		bool needSeparator = false;
		bool takeAwaySeparator = false;
		if (!IsSeparator(other.m_Filepath[0]) && m_Filepath.size() > 0 && !IsSeparator(m_Filepath[m_Filepath.size() - 1]))
		{
			needSeparator = true;
			newLength++;
		}
		else if (IsSeparator(other.m_Filepath[0]) && m_Filepath.size() > 0 && IsSeparator(m_Filepath[m_Filepath.size() - 1]))
		{
			takeAwaySeparator = true;
			newLength--;
		}

		if (!needSeparator && !takeAwaySeparator)
		{
			// If only one of the paths has a separator at the end or beginning, just str combine
			// and return
			this->m_Filepath += other.m_Filepath;
			return;
		}

		if (needSeparator)
		{
			this->m_Filepath += m_PathSeparator;
			this->m_Filepath += other.m_Filepath;
		}
		else if (takeAwaySeparator)
		{
			this->m_Filepath += (other.m_Filepath.c_str() + 1);
		}

		m_FilenameOffset = m_Filepath.size() - (other.m_Filepath.size() - other.m_FilenameOffset);
		m_FileExtOffset = m_Filepath.size() - (other.m_Filepath.size() - other.m_FileExtOffset);
	}

	/*
	 *  Gets the directory at 'level' of the path. For example given the directory:
	 *
	 *   0  1   2        3           4   Undefined
	 *  'C:/dev/Projects/SomeProject/Src/Somefile.txt'
	 *   -5 -4  -3       -2          -1  Undefined
	 *
	 *  The levels are listed above and below the path respectively
	 *  So, to get a directory one path level above the current directory, you would simply say:
	 *  GetDirectory(-2);
	 */
	std::string CPath::GetDirectory(int level) const
	{
		const char* startCopy = m_Filepath.c_str();
		const char* endCopy = m_Filepath.c_str();
		if (level < 0)
		{

			for (int i = (int)(m_Filepath.size() - 1); i >= 0; i--)
			{
				if (IsSeparator(m_Filepath[i]))
				{
					level++;
					if (level >= 0)
					{
						endCopy = &m_Filepath[i];
						break;
					}
				}
			}
		}
		else
		{
			for (int i = 0; i < m_Filepath.size(); i++)
			{
				if (IsSeparator(m_Filepath[i]))
				{
					level--;
					if (level < 0)
					{
						endCopy = &m_Filepath[i];
						if (IsFile() && endCopy == &m_Filepath[m_FilenameOffset])
						{
							endCopy = m_Filepath.c_str();
						}
						break;
					}
				}
			}
		}

		return std::string(startCopy, endCopy);
	}

	bool CPath::IsFile() const
	{
		return IFile::IsFile(*this);
	}

	bool CPath::IsDirectory()
	{
		return IFile::IsDirectory(*this);
	}

	std::string CPath::GetFilenameWithoutExt() const
	{
		return m_Filepath.substr(m_FilenameOffset, FilenameSize() - FileExtSize());
	}
}