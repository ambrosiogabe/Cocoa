#ifndef COCOA_EDITOR_SOURCE_FILE_WATCHER_H
#define COCOA_EDITOR_SOURCE_FILE_WATCHER_H
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "cocoa/file/FileSystemWatcher.h"

namespace Cocoa
{
	class SourceFileWatcher
	{
	public:
		SourceFileWatcher() 
			: mRootDirectory("") { }
		SourceFileWatcher(const std::filesystem::path& path);
		~SourceFileWatcher() { mFileWatcher.stop(); }

	private:
		void startFileWatcher();

	private:
		std::filesystem::path mRootDirectory;
		FileSystemWatcher mFileWatcher;
	};
}

#endif
