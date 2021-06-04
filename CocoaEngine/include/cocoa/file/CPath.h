#pragma once
#include "externalLibs.h"

namespace Cocoa
{
	struct CPath
	{
		// TODO: TEST THIS A LOT
		const char* Path;
		int FilenameOffset;
		int FileExtOffset;
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

		COCOA std::string LinuxStyle(const CPath& path);
	};

	COCOA bool operator==(const CPath& a, const CPath& b);
}