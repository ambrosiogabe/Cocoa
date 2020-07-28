#include "jade/file/JPath.h"
#include "jade/util/Log.h"

namespace Jade
{
#ifdef _WIN32
	char JPath::PATH_SEPARATOR = Win32JPath::GetPathSeparator();
#endif

	JPath::JPath(std::string path)
	{
		char* sanitizedPath = new char[path.size()];
		int lastPathSeparator = -1;
		int stringIndex = 0;
		int pathIndex = 0;
		int lastDot = -1;

		for (char c : path)
		{
			if (c == '.' && stringIndex > 0 && sanitizedPath[stringIndex - 1] != '.')
			{
				lastDot = pathIndex;
			}
			else if (c == '.' && stringIndex > 0 && sanitizedPath[stringIndex - 1] == '.')
			{
				// We have a double dot ..
				// If .. is at the end of the path or .. is followed by a path separator '/'
				if (stringIndex == path.size() - 1 || (stringIndex + 1 < path.size() && IsSeparator(path[stringIndex + 1])))
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

	JPath::~JPath()
	{
		delete[] m_Filepath;
	}

	JPath JPath::operator+(std::string other)
	{
		JPath tmp(other);
		JPath copy = JPath(*this);
		copy.Join(tmp);
		return copy;
	}

	JPath JPath::operator+(const JPath& other)
	{
		JPath copy = JPath(*this);
		copy.Join(other);
		return copy;
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
}