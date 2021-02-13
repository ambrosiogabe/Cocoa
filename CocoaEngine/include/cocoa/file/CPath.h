#pragma once
#include "externalLibs.h"

namespace Cocoa
{
	struct CPath
	{
		// TODO: Come up with immutable string type and switch this to that
		std::string Path;
		int FilenameOffset = 0;
		int FileExtOffset = 0;
	};

	namespace NCPath
	{
		COCOA CPath CreatePath(bool forceLinuxStylePath = false);
		COCOA CPath CreatePath(const std::string& path, bool forceLinuxStylePath = false);
		COCOA CPath CreatePath(const char* path, bool forceLinuxStylePath = false);

		COCOA int FilenameSize(const CPath& path);
		COCOA int FileExtSize(const CPath& path);
		COCOA int Size(const CPath& path);

		COCOA const char* Filename(const CPath& path);
		COCOA const char* Filepath(const CPath& path);
		COCOA const char* FileExt(const CPath& path);

		COCOA std::string GetDirectory(const CPath& path, int level);
		COCOA std::string GetFilenameWithoutExt(const CPath& path);

		COCOA void Join(CPath& pathToJoin, const CPath& other);
		COCOA bool Contains(const CPath& path, const char* pathSegment);
	};

	COCOA bool operator==(const CPath& a, const CPath& b);
}