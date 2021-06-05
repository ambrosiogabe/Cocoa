#include "cocoa/file/CPath.h"
#include "cocoa/file/File.h"

namespace Cocoa
{
	// Internal Variables
	static char WIN_SEPARATOR = '\\';
	static char UNIX_SEPARATOR = '/';
#ifdef _WIN32
	static char PATH_SEPARATOR = WIN_SEPARATOR;
#endif

	// Forward Declarations
	static bool IsSeparator(char c);
	static void Init(CPath& outPath, const char* path, int pathSize);

	CPath CPath::create(bool forceLinuxStylePath)
	{
		return CPath{ String::CreateString(""), 0, 0 };
	}

	CPath CPath::create(const std::string& rawPath, bool forceLinuxStylePath)
	{
		CPath path;
		Init(path, rawPath.c_str(), (int)rawPath.size());
		return path;
	}

	CPath CPath::create(const char* rawPath, bool forceLinuxStylePath)
	{
		CPath path;
		int pathSize = 0;
		if (rawPath != nullptr)
		{
			for (const char* iter = rawPath; *iter != '\0'; iter++)
			{
				pathSize++;
			}
		}

		Init(path, rawPath, pathSize);
		return path;
	}

	int CPath::filenameSize() const
	{
		return (int)(String::StringLength(path) - filenameOffset);
	}

	int CPath::fileExtSize() const
	{
		return (int)(String::StringLength(path) - fileExtOffset);
	}

	int CPath::size() const
	{
		return (int)String::StringLength(path);
	}

	const char* CPath::filename() const
	{
		return &path[filenameOffset];
	}

	const char* CPath::filepath() const
	{
		return path;
	}

	const char* CPath::fileExt() const
	{
		return &path[fileExtOffset];
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
	std::string CPath::getDirectory(int level) const
	{
		const char* startCopy = path;
		const char* endCopy = path;
		int pathSize = String::StringLength(path);
		if (level < 0)
		{
			for (int i = pathSize - 1; i >= 0; i--)
			{
				if (IsSeparator(path[i]))
				{
					level++;
					if (level >= 0)
					{
						endCopy = &path[i];
						break;
					}
				}
			}
		}
		else
		{
			for (int i = 0; i < pathSize; i++)
			{
				if (IsSeparator(path[i]))
				{
					level--;
					if (level < 0)
					{
						endCopy = &path[i];
						if (File::isFile(*this) && endCopy == &path[filenameOffset])
						{
							// If we reach the beginning of the string then set the end of the copy to the string beginning
							// so that nothing is copied
							endCopy = path;
						}
						break;
					}
				}
			}
		}

		return std::string(startCopy, endCopy);
	}

	std::string CPath::getFilenameWithoutExt() const
	{
		return String::Substring(path, filenameOffset, filenameSize() - fileExtSize());
	}

	void CPath::join(const CPath& other)
	{
		int otherPathSize = String::StringLength(other.path);
		int pathToJoinSize = String::StringLength(path);
		int newLength = (int)(otherPathSize + pathToJoinSize);
		bool needSeparator = false;
		bool takeAwaySeparator = false;
		if (!IsSeparator(other.path[0]) && pathToJoinSize > 0 && !IsSeparator(path[pathToJoinSize - 1]))
		{
			needSeparator = true;
			newLength++;
		}
		else if (IsSeparator(other.path[0]) && pathToJoinSize > 0 && IsSeparator(path[pathToJoinSize - 1]))
		{
			takeAwaySeparator = true;
			newLength--;
		}

		StringBuilder sb;
		sb.Append(path);
		if (!needSeparator && !takeAwaySeparator)
		{
			// If only one of the paths has a separator at the end or beginning, just str combine
			// and return
			sb.Append(other.path);
			return;
		} else if (needSeparator)
		{
			sb.Append(PATH_SEPARATOR);
			sb.Append(other.path);
		}
		else if (takeAwaySeparator)
		{
			sb.Append((other.path + 1));
		}

		//String::FreeString(pathToJoin.Path);
		path = sb.c_str_copy();
		pathToJoinSize = String::StringLength(path);
		otherPathSize = String::StringLength(other.path);
		filenameOffset = pathToJoinSize - (otherPathSize - other.filenameOffset);
		fileExtOffset = pathToJoinSize - (otherPathSize - other.fileExtOffset);
	}

	bool CPath::contains(const char* pathSegment) const
	{
		return std::string(path).find(pathSegment) != std::string::npos;
	}

	const char* CPath::linuxStyle() const
	{
		StringBuilder res;
		int stringLength = size();
		for (int i=0; i < stringLength; i++)
		{
			char c = path[i];
			if (c == WIN_SEPARATOR)
			{
				res.Append('/');
			}
			else
			{
				res.Append(c);
			}
		}

		return res.c_str_copy();
	}

	// Internal functions
	static bool IsSeparator(char c)
	{
		return (c == WIN_SEPARATOR || c == UNIX_SEPARATOR);
	}

	static void Init(CPath& outPath, const char* path, int pathSize)
	{
		// TODO: Should be enough characters for any valid path... Maybe make this platform dependent in the future
		static char pathBuffer[260];

		if (pathSize == 0)
		{
			outPath.path = "";
			outPath.fileExtOffset = 0;
			outPath.filenameOffset = 0;
			return;
		}

		int lastPathSeparator = -1;
		int stringIndex = 0;
		int pathIndex = 0;
		int lastDot = -1;

		const char* iterEnd = path + pathSize;
		for (const char* iter = path; iter != iterEnd; iter++)
		{
			char c = *iter;
			if (c == '.' && stringIndex > 0 && pathBuffer[stringIndex - 1] != '.')
			{
				lastDot = pathIndex;
			}
			else if (c == '.' && stringIndex > 0 && pathBuffer[stringIndex - 1] == '.')
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

			if (IsSeparator(c) && (stringIndex == 0 || stringIndex == 1 || !IsSeparator(pathBuffer[stringIndex - 1])))
			{
				pathBuffer[pathIndex] = PATH_SEPARATOR;
				lastPathSeparator = pathIndex;
				pathIndex++;
			}
			else if (!IsSeparator(c))
			{
				pathBuffer[pathIndex] = c;
				pathIndex++;
			}

			stringIndex++;
		}
		pathBuffer[pathIndex] = '\0';

		// TODO: Bug here (repro just open an existing project possibly with a blank name for example '.cprj')
		outPath.path = String::CreateString(pathBuffer);
		outPath.filenameOffset = lastPathSeparator >= -1 && lastPathSeparator != pathIndex
			? lastPathSeparator + 1 : pathIndex;
		outPath.fileExtOffset = (lastDot == -1 || lastDot < lastPathSeparator || lastPathSeparator == lastDot - 1)
			? pathIndex : lastDot;
	}

	bool operator==(const CPath& a, const CPath& b)
	{
		if (a.size() == 0 || b.size() == 0)
		{
			return false;
		}

		CPath tmp1 = File::getAbsolutePath(a);
		CPath tmp2 = File::getAbsolutePath(b);
		return String::Compare(tmp1.path, tmp2.path) == 0;
	}
}