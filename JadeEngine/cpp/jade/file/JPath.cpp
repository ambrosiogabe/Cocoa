#include "jade/file/JPath.h"
#include "jade/util/Log.h"
#include "jade/file/IFile.h"

namespace Jade
{
#ifdef _WIN32
	char JPath::PATH_SEPARATOR = Win32JPath::GetPathSeparator();
#endif

	JPath::JPath(std::string path)
	{
		Init(path.c_str(), path.size());
	}

	JPath::JPath(const char* path)
	{
		int pathSize = 0;
		for (const char* iter = path; *iter != '\0'; iter++)
		{
			pathSize++;
		}

		Init(path, pathSize);
	}

	JPath::JPath(const JPath& other)
	{
		m_Filepath = other.m_Filepath;
		m_Filename = &this->m_Filepath[other.m_Filename - other.m_Filepath.c_str()];
		m_FileExt = &this->m_Filepath[other.m_FileExt - other.m_Filepath.c_str()];
	}

	void JPath::Init(const char* path, int pathSize)
	{
		if (pathSize == 0)
		{
			m_Filepath = "";
			m_FileExt = "";
			m_Filename = "";
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
				sanitizedPath[pathIndex] = PATH_SEPARATOR;
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
		m_Filename = lastPathSeparator >= -1 && lastPathSeparator != pathIndex
			? &m_Filepath[lastPathSeparator + 1] : &m_Filepath[pathIndex];
		m_FileExt = (lastDot == -1 || lastDot < lastPathSeparator || lastPathSeparator == lastDot - 1)
			? &m_Filepath[pathIndex] : &m_Filepath[lastDot];

		delete[] sanitizedPath;
	}

	JPath::JPath(JPath&& other) noexcept
		: m_Filepath(std::move(other.m_Filepath))
	{
		m_Filename = other.m_Filename;
		m_FileExt = other.m_FileExt;
	}

	JPath& JPath::operator=(JPath&& other) noexcept
	{
		if (this != &other)
		{
			// Free the existing resource

			// Copy the data pointers from other to here
			m_Filepath = std::move(other.m_Filepath);
			m_FileExt = other.m_FileExt;
			m_Filename = other.m_Filename;

			// Release the data pointers from source, so destructor does not free multiple times
			other.m_Filename = nullptr;
			other.m_FileExt = nullptr;
		}

		return *this;
	}

	JPath& JPath::operator=(JPath& other)
	{
		if (&other != this)
		{
			m_Filepath = other.m_Filepath;
			m_Filename = &m_Filepath[other.m_Filename - other.m_Filepath.c_str()];
			m_FileExt = &m_Filepath[other.m_FileExt - other.m_Filepath.c_str()];
		}

		return *this;
	}

	JPath::~JPath()
	{
		m_Filename = nullptr;
		m_FileExt = nullptr;
	}

	JPath JPath::operator+(std::string other) const
	{
		JPath copy = JPath(*this);
		copy.Join(JPath(other));
		return copy;
	}

	JPath JPath::operator+(const char* other) const
	{
		JPath copy = JPath(*this);
		copy.Join(JPath(other));
		return copy;
	}

	JPath JPath::operator+(const JPath& other) const
	{
		JPath copy = JPath(*this);
		copy.Join(other);
		return copy;
	}

	void JPath::operator+=(const JPath& other)
	{
		this->Join(other);
	}

	bool JPath::operator==(const JPath& other) const
	{
		if (m_Filepath.size() == 0 || other.m_Filepath.size() == 0)
		{
			return false;
		}

		JPath tmp1 = IFile::GetAbsolutePath(other);
		JPath tmp2 = IFile::GetAbsolutePath(*this);
		return strcmp(tmp1.m_Filepath.c_str(), tmp2.m_Filepath.c_str()) == 0;
	}

	char JPath::operator[](int index) const
	{
		return m_Filepath[index];
	}

	void JPath::Join(const JPath& other)
	{
		int newLength = other.m_Filepath.size() + m_Filepath.size();
		bool needSeparator = false;
		bool takeAwaySeparator = false;
		if (!IsSeparator(other.m_Filepath[0]) && !IsSeparator(m_Filepath[m_Filepath.size() - 1]))
		{
			needSeparator = true;
			newLength++;
		}
		else if (IsSeparator(other.m_Filepath[0]) && IsSeparator(m_Filepath[m_Filepath.size() - 1]))
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
			this->m_Filepath += PATH_SEPARATOR;
			this->m_Filepath += other.m_Filepath;
		}
		else if (takeAwaySeparator)
		{
			this->m_Filepath += (other.m_Filepath.c_str() + 1);
		}

		m_Filename = &m_Filepath[m_Filepath.size() - (other.m_Filename - other.m_Filepath.c_str())];
		m_FileExt = &m_Filepath[m_Filepath.size() - (other.m_FileExt - other.m_Filepath.c_str())];
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
	std::string JPath::GetDirectory(int level)
	{
		const char* startCopy = m_Filepath.c_str();
		const char* endCopy = m_Filepath.c_str();
		if (level < 0)
		{
			if (IsFile())
			{
				level--;
			}

			for (int i = m_Filepath.size() - 1; i >= 0; i--)
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
						if (IsFile() && endCopy == m_Filename)
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

	bool JPath::IsFile()
	{
		return IFile::IsFile(*this);
	}

	bool JPath::IsDirectory()
	{
		return IFile::IsDirectory(*this);
	}
}