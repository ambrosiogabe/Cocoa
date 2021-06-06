#include "cocoa/file/Path.h"
#include "cocoa/file/File.h"

namespace Cocoa
{
	// Internal Variables
	static const char WIN_SEPARATOR = '\\';
	static const char UNIX_SEPARATOR = '/';
#ifdef _WIN32
	static const char PATH_SEPARATOR = WIN_SEPARATOR;
#endif

	// Forward Declarations
	static bool isSeparator(char c);

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
	std::string Path::getDirectory(int level) const
	{
		const char* startCopy = path;
		const char* endCopy = path;
		int pathSize = String::StringLength(path);
		if (level < 0)
		{
			for (int i = pathSize - 1; i >= 0; i--)
			{
				if (isSeparator(path[i]))
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
				if (isSeparator(path[i]))
				{
					level--;
					if (level < 0)
					{
						endCopy = &path[i];
						if (File::isFile(*this) && endCopy == filename)
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

	std::string Path::getFilenameWithoutExt() const
	{
		return String::Substring(path, size - filenameSize, filenameSize - fileExtSize);
	}

	bool Path::contains(const char* pathSegment) const
	{
		return std::string(path).find(pathSegment) != std::string::npos;
	}

	const char* Path::linuxStyle() const
	{
		StringBuilder res;
		const int stringLength = size;
		for (int i=0; i < stringLength; i++)
		{
			const char c = path[i];
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

	Path Path::createDefault()
	{
		Path res;
		ZeroMemory(&res, sizeof(Path));
		return res;
	}

	// Internal functions
	static bool isSeparator(char c)
	{
		return (c == WIN_SEPARATOR || c == UNIX_SEPARATOR);
	}

	bool operator==(const Path& a, const Path& b)
	{
		if (a.size == 0 || b.size == 0)
		{
			return false;
		}

		Path tmp1 = File::getAbsolutePath(a);
		Path tmp2 = File::getAbsolutePath(b);
		return String::Compare(tmp1.path, tmp2.path) == 0;
	}

	// =====================================================
	// PathBuilder
	// =====================================================

	PathBuilder::PathBuilder()
	{
		mFileExtOffset = 0;
		mFilenameOffset = 0;
		mPath = List<char>();
	}
	
	PathBuilder::PathBuilder(const char* path)
	{
		init(path);
	}
	
	PathBuilder::PathBuilder(const Path& path)
	{
		init(path.path);
	}

	PathBuilder& PathBuilder::join(const char* other)
	{
		const Path otherPath = PathBuilder(other).createPath();
		join(otherPath);
		String::FreeString(otherPath.path);
		return *this;
	}
	
	PathBuilder& PathBuilder::join(const Path& other)
	{
		const int pathToJoinSize = mPath.size();

		bool needSeparator = false;
		bool takeAwaySeparator = false;
		if (!isSeparator(other.path[0]) && pathToJoinSize > 0 && !isSeparator(mPath[pathToJoinSize - 1]))
		{
			needSeparator = true;
		}
		else if (isSeparator(other.path[0]) && pathToJoinSize > 0 && isSeparator(mPath[pathToJoinSize - 1]))
		{
			takeAwaySeparator = true;
		}

		StringBuilder sb;
		mPath.push('\0');
		sb.Append(mPath.begin());
		mPath.pop();

		if (!needSeparator && !takeAwaySeparator)
		{
			// If only one of the paths has a separator at the end or beginning, just str combine
			sb.Append(other.path);
		}
		else if (needSeparator)
		{
			// If we need the separator, add a separator then add the other path
			sb.Append(PATH_SEPARATOR);
			sb.Append(other.path);
		}
		else if (takeAwaySeparator)
		{
			// Otherwise just take the other path after the first character
			sb.Append(other.path + 1);
		}

		mPath.replace(0, sb.c_str(), sb.Size());
		const int newPathSize = mPath.size();
		const int otherPathSize = String::StringLength(other.path);
		mFilenameOffset = newPathSize - other.filenameSize;
		mFileExtOffset = newPathSize - other.fileExtSize;

		return *this;
	}

	Path PathBuilder::createPath()
	{
		mPath.push('\0');
		const char* copy = String::CreateString(mPath.begin());
		mPath.pop();
		
		Path res;
		res.path = copy;
		res.filename = &copy[mFilenameOffset];
		res.fileExt = &copy[mFileExtOffset];
		res.filenameSize = mPath.size() - mFilenameOffset;
		res.fileExtSize = mPath.size() - mFileExtOffset;
		res.size = mPath.size();
		return res;
	}

	Path PathBuilder::createTmpPath()
	{
		mPath.push('\0');
		const char* shallowRef = mPath.begin();
		mPath.pop();

		Path res;
		res.path = shallowRef;
		res.filename = &shallowRef[mFilenameOffset];
		res.fileExt = &shallowRef[mFileExtOffset];
		res.filenameSize = mPath.size() - mFilenameOffset;
		res.fileExtSize = mPath.size() - mFileExtOffset;
		res.size = mPath.size();
		return res;
	}

	const char* PathBuilder::c_str()
	{
		mPath.push('\0');
		const char* copy = String::CreateString(mPath.begin());
		mPath.pop();

		return mPath.begin();
	}

	void PathBuilder::init(const char* path)
	{
		const int pathSize = String::StringLength(path);
		mPath = List<char>();

		if (pathSize == 0)
		{
			mFilenameOffset = 0;
			mFileExtOffset = 0;
			return;
		}

		int lastPathSeparator = -1;
		int lastDot = -1;
		for (int i = 0; i < pathSize; i++)
		{
			char c = path[i];
			if (c == '.' && i > 0 && mPath[i - 1] != '.')
			{
				lastDot = mPath.size();
			}
			else if (c == '.' && i > 0 && mPath[i - 1] == '.')
			{
				// We have a double dot ..
				if (i == pathSize - 1 || (i + 1 < pathSize && isSeparator(path[i + 1])))
				{
					// If .. is at the end of the path or .. is followed by a path separator '/' don't count it as the last dot
					lastDot = -1;
				}
				else
				{
					// Otherwise count this dot as the last dot
					lastDot = mPath.size();
				}
			}

			if (isSeparator(c) && (i == 0 || i == 1 || !isSeparator(mPath[i - 1])))
			{
				lastPathSeparator = mPath.size();
				mPath.push(PATH_SEPARATOR);
			}
			else if (!isSeparator(c))
			{
				mPath.push(c);
			}
		}

		// Set the filename offset to the last path separator, or to the null byte which is the size of the path
		mFilenameOffset = 
			lastPathSeparator >= -1 && lastPathSeparator < mPath.size()
			? lastPathSeparator + 1
			: mPath.size();
		// Set the file extension offset to the last dot, or to the null byte which is the size of the path
		mFileExtOffset = 
			(lastDot == -1 || lastDot < lastPathSeparator || lastPathSeparator == lastDot - 1)
			? mPath.size()
			: lastDot;
	}
}