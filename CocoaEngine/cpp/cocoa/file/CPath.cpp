#include "cocoa/file/CPath.h"
#include "cocoa/file/File.h"

namespace Cocoa
{
	namespace NCPath
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

		CPath CreatePath(bool forceLinuxStylePath)
		{
			return CPath{ String::CreateString(""), 0, 0 };
		}

		CPath CreatePath(const std::string& rawPath, bool forceLinuxStylePath)
		{
			CPath path;
			Init(path, rawPath.c_str(), (int)rawPath.size());
			return path;
		}

		CPath CreatePath(const char* rawPath, bool forceLinuxStylePath)
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

		int FilenameSize(const CPath& path)
		{
			return (int)(String::StringLength(path.Path) - path.FilenameOffset);
		}

		int FileExtSize(const CPath& path)
		{
			return (int)(String::StringLength(path.Path) - path.FileExtOffset);
		}

		int Size(const CPath& path)
		{
			return (int)String::StringLength(path.Path);
		}

		const char* Filename(const CPath& path)
		{
			return &path.Path[path.FilenameOffset];
		}

		const char* Filepath(const CPath& path)
		{
			return path.Path;
		}

		const char* FileExt(const CPath& path)
		{
			return &path.Path[path.FileExtOffset];
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
		std::string GetDirectory(const CPath& path, int level)
		{
			const char* startCopy = path.Path;
			const char* endCopy = path.Path;
			int pathSize = String::StringLength(path.Path);
			if (level < 0)
			{
				for (int i = pathSize - 1; i >= 0; i--)
				{
					if (IsSeparator(path.Path[i]))
					{
						level++;
						if (level >= 0)
						{
							endCopy = &path.Path[i];
							break;
						}
					}
				}
			}
			else
			{
				for (int i = 0; i < pathSize; i++)
				{
					if (IsSeparator(path.Path[i]))
					{
						level--;
						if (level < 0)
						{
							endCopy = &path.Path[i];
							if (File::IsFile(path) && endCopy == &path.Path[path.FilenameOffset])
							{
								// If we reach the beginning of the string then set the end of the copy to the string beginning
								// so that nothing is copied
								endCopy = path.Path;
							}
							break;
						}
					}
				}
			}

			return std::string(startCopy, endCopy);
		}

		std::string GetFilenameWithoutExt(const CPath& path)
		{
			return String::Substring(path.Path, path.FilenameOffset, FilenameSize(path) - FileExtSize(path));
		}

		void Join(CPath& pathToJoin, const CPath& other)
		{
			int otherPathSize = String::StringLength(other.Path);
			int pathToJoinSize = String::StringLength(pathToJoin.Path);
			int newLength = (int)(otherPathSize + pathToJoinSize);
			bool needSeparator = false;
			bool takeAwaySeparator = false;
			if (!IsSeparator(other.Path[0]) && pathToJoinSize > 0 && !IsSeparator(pathToJoin.Path[pathToJoinSize - 1]))
			{
				needSeparator = true;
				newLength++;
			}
			else if (IsSeparator(other.Path[0]) && pathToJoinSize > 0 && IsSeparator(pathToJoin.Path[pathToJoinSize - 1]))
			{
				takeAwaySeparator = true;
				newLength--;
			}

			StringBuilder sb;
			sb.Append(pathToJoin.Path);
			if (!needSeparator && !takeAwaySeparator)
			{
				// If only one of the paths has a separator at the end or beginning, just str combine
				// and return
				sb.Append(other.Path);
				return;
			} else if (needSeparator)
			{
				sb.Append(PATH_SEPARATOR);
				sb.Append(other.Path);
			}
			else if (takeAwaySeparator)
			{
				sb.Append((other.Path + 1));
			}

			String::FreeString(pathToJoin.Path);
			pathToJoin.Path = sb.c_str_copy();
			pathToJoinSize = String::StringLength(pathToJoin.Path);
			otherPathSize = String::StringLength(other.Path);
			pathToJoin.FilenameOffset = pathToJoinSize - (otherPathSize - other.FilenameOffset);
			pathToJoin.FileExtOffset = pathToJoinSize - (otherPathSize - other.FileExtOffset);
		}

		bool Contains(const CPath& path, const char* pathSegment)
		{
			return std::string(path.Path).find(pathSegment) != std::string::npos;
		}

		std::string LinuxStyle(const CPath& path)
		{
			std::string res = path.Path;
			int stringLength = (int)res.size();
			for (int i=0; i < stringLength; i++)
			{
				char c = res[i];
				if (c == WIN_SEPARATOR)
				{
					res[i] = '/';
				}
			}

			return res;
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
				outPath.Path = "";
				outPath.FileExtOffset = 0;
				outPath.FilenameOffset = 0;
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
			outPath.Path = String::CreateString(pathBuffer);
			outPath.FilenameOffset = lastPathSeparator >= -1 && lastPathSeparator != pathIndex
				? lastPathSeparator + 1 : pathIndex;
			outPath.FileExtOffset = (lastDot == -1 || lastDot < lastPathSeparator || lastPathSeparator == lastDot - 1)
				? pathIndex : lastDot;
		}
	}

	bool operator==(const CPath& a, const CPath& b)
	{
		if (NCPath::Size(a) == 0 || NCPath::Size(b) == 0)
		{
			return false;
		}

		CPath tmp1 = File::GetAbsolutePath(a);
		CPath tmp2 = File::GetAbsolutePath(b);
		return String::Compare(tmp1.Path, tmp2.Path) == 0;
	}
}