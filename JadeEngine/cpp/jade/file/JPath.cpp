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
		m_PathLength = other.m_PathLength;
		m_Filepath = new char[m_PathLength + 1];
		m_Filepath[m_PathLength] = '\0';
		int result = memcpy_s(m_Filepath, m_PathLength, other.m_Filepath, m_PathLength);
		Log::Assert(result == 0, "Failed to copy path.");
		m_Filename = &this->m_Filepath[other.m_Filename - other.m_Filepath];
		m_FileExt = &this->m_Filepath[other.m_FileExt - other.m_Filepath];
	}

	void JPath::Init(const char* path, int pathSize)
	{
		char* sanitizedPath = new char[pathSize];
		int lastPathSeparator = -1;
		int stringIndex = 0;
		int pathIndex = 0;
		int lastDot = -1;

		const char* iterEnd = path + pathSize;
		for (const char* iter=path; iter != iterEnd; iter++)
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

		m_Filepath = new char[pathIndex + 1];
		int success = memcpy_s(m_Filepath, pathIndex + 1, sanitizedPath, pathIndex);
		Log::Assert(success == 0, "Error copying filepath.");
		m_Filepath[pathIndex] = '\0';
		m_Filename = lastPathSeparator >= -1 && lastPathSeparator != pathIndex
			? &m_Filepath[lastPathSeparator + 1] : &m_Filepath[pathIndex];
		m_FileExt = (lastDot == -1 || lastDot < lastPathSeparator || lastPathSeparator == lastDot - 1)
			? &m_Filepath[pathIndex] : &m_Filepath[lastDot];
		m_PathLength = pathIndex;

		delete[] sanitizedPath;
	}

	JPath::JPath(JPath&& other) noexcept
	{
		m_Filepath = other.m_Filepath;
		m_Filename = other.m_Filename;
		m_FileExt = other.m_FileExt;
		m_PathLength = other.m_PathLength;

		other.m_Filepath = nullptr;
		other.m_Filename = nullptr;
		other.m_FileExt = nullptr;
		other.m_PathLength = 0;
	}

	JPath& JPath::operator=(JPath&& other) noexcept
	{
		if (this != &other)
		{
			// Free the existing resource
			delete m_Filepath;

			// Copy the data pointers from other to here
			m_Filepath = other.m_Filepath;
			m_FileExt = other.m_FileExt;
			m_Filename = other.m_Filename;
			m_PathLength = other.m_PathLength;

			// Release the data pointers from source, so destructor does not free multiple times
			other.m_Filepath = nullptr;
			other.m_Filename = nullptr;
			other.m_FileExt = nullptr;
			other.m_PathLength = 0;
		}

		return *this;
	}

	JPath::~JPath()
	{
		if (m_Filepath != nullptr)
			delete[] m_Filepath;
	}

	JPath& JPath::operator=(JPath& other)
	{
		if (&other == this)
			return *this;
		if (other.m_PathLength != m_PathLength)
		{
			delete[] m_Filepath;
			m_Filepath = new char[other.m_PathLength + 1];
			m_PathLength = other.m_PathLength;
		}

		memcpy_s(m_Filepath, m_PathLength, other.m_Filepath, other.m_PathLength);
		m_Filename = &m_Filepath[other.m_Filename - other.m_Filepath];
		m_FileExt = &m_Filepath[other.m_FileExt - other.m_Filepath];
		return *this;
	}

	JPath JPath::operator+(std::string other) const
	{
		JPath tmp(other);
		JPath copy = JPath(*this);
		copy.Join(tmp);
		return copy;
	}

	JPath JPath::operator+(const JPath& other) const
	{
		JPath copy = JPath(*this);
		copy.Join(other);
		return copy;
	}

	JPath JPath::operator+(const char* other) const
	{
		JPath copy = JPath(*this);
		copy.Join(JPath(other));
		return copy;
	}

	void JPath::operator+=(const JPath& other)
	{
		this->Join(other);
	}

	char JPath::operator[](int index) const
	{
		return m_Filepath[index];
	}

	void JPath::Join(const JPath& other)
	{
		int newLength = other.m_PathLength + m_PathLength;
		bool needSeparator = false;
		bool takeAwaySeparator = false;
		if (!IsSeparator(other.m_Filepath[0]) && !IsSeparator(m_Filepath[m_PathLength - 1]))
		{
			needSeparator = true;
			newLength++;
		}
		else if (IsSeparator(other.m_Filepath[0]) && IsSeparator(m_Filepath[m_PathLength - 1]))
		{
			takeAwaySeparator = true;
			newLength--;
		}
		char* newBuffer = new char[newLength + 1];
		char* startSecond = &newBuffer[m_PathLength];
		if (needSeparator)
		{
			*startSecond = PATH_SEPARATOR;
			startSecond++;
		}
		else if (takeAwaySeparator)
		{
			startSecond--;
		}

		memcpy_s(newBuffer, m_PathLength, m_Filepath, m_PathLength);
		memcpy_s(startSecond, other.m_PathLength, other.m_Filepath, other.m_PathLength);
		delete[] m_Filepath;
		m_Filepath = newBuffer;
		m_Filename = &newBuffer[m_PathLength + (other.m_Filename - other.m_Filepath)];
		m_FileExt = &newBuffer[m_PathLength + (other.m_FileExt - other.m_Filepath)];
		m_Filepath[newLength] = '\0';
		m_PathLength = newLength;
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
		char* startCopy = m_Filepath;
		char* endCopy = m_Filepath;
		if (level < 0)
		{
			if (IsFile())
			{
				level--;
			}

			for (int i = m_PathLength - 1; i >= 0; i--)
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
			for (int i = 0; i < m_PathLength; i++)
			{
				if (IsSeparator(m_Filepath[i]))
				{
					level--;
					if (level < 0)
					{
						endCopy = &m_Filepath[i];
						if (IsFile() && endCopy == m_Filename)
						{
							endCopy = m_Filepath;
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