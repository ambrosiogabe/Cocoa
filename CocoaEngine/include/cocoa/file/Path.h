#ifndef COCOA_ENGINE_PATH_H
#define COCOA_ENGINE_PATH_H
#include "externalLibs.h"

namespace Cocoa
{
	// TODO: Make a path builder that is not POD and make the Path constant
	struct COCOA Path
	{
		// TODO: TEST THIS A LOT
		const char* path;
		const char* filename;
		const char* fileExt;
		int filenameSize;
		int fileExtSize;
		int size;

		std::string getDirectory(int level) const;
		std::string getFilenameWithoutExt() const;
		bool contains(const char* pathSegment) const;

		const char* linuxStyle() const;

		static Path createDefault();
	};

	COCOA bool operator==(const Path& a, const Path& b);

	class COCOA PathBuilder
	{
	public:
		PathBuilder();
		PathBuilder(const char* path);
		PathBuilder(const Path& path);

		PathBuilder& join(const char* other);
		PathBuilder& join(const Path& other);

		Path createPath();
		Path createTmpPath();
		const char* c_str();

	private:
		void init(const char* path);

	private:
		List<char> mPath;
		int mFileExtOffset;
		int mFilenameOffset;
	};
}

#endif
