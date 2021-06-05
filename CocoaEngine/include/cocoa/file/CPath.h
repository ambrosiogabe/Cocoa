#ifndef COCOA_ENGINE_CPATH_H
#define COCOA_ENGINE_CPATH_H
#include "externalLibs.h"

namespace Cocoa
{
	// TODO: Make a path builder that is not POD and make the CPath constant
	struct COCOA CPath
	{
		// TODO: TEST THIS A LOT
		const char* path;
		int filenameOffset;
		int fileExtOffset;

		static CPath create(bool forceLinuxStylePath = false);
		static CPath create(const std::string& path, bool forceLinuxStylePath = false);
		static CPath create(const char* path, bool forceLinuxStylePath = false);

		int filenameSize() const;
		int fileExtSize() const;
		int size() const;

		const char* filename() const;
		const char* filepath() const;
		const char* fileExt() const;

		std::string getDirectory(int level) const;
		std::string getFilenameWithoutExt() const;

		void join(const CPath& other);
		bool contains(const char* pathSegment) const;

		const char* linuxStyle() const;
	};

	COCOA bool operator==(const CPath& a, const CPath& b);
}

#endif
