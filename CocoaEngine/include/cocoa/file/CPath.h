#pragma once
#include "externalLibs.h"

namespace Cocoa
{
	// TODO: Make a path builder that is not POD and make the CPath constant
	struct COCOA CPath
	{
		// TODO: TEST THIS A LOT
		const char* Path;
		int FilenameOffset;
		int FileExtOffset;

		static CPath Create(bool forceLinuxStylePath = false);
		static CPath Create(const std::string& path, bool forceLinuxStylePath = false);
		static CPath Create(const char* path, bool forceLinuxStylePath = false);

		int FilenameSize() const;
		int FileExtSize() const;
		int Size() const;

		const char* Filename() const;
		const char* Filepath() const;
		const char* FileExt() const;

		std::string GetDirectory(int level) const;
		std::string GetFilenameWithoutExt() const;

		void Join(const CPath& other);
		bool Contains(const char* pathSegment) const;

		const char* LinuxStyle() const;
	};

	COCOA bool operator==(const CPath& a, const CPath& b);
}