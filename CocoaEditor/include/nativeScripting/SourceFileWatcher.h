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
			: mRootDirectory(CPath::create()) {}
		SourceFileWatcher(CPath path);
		~SourceFileWatcher() { mFileWatcher.stop(); }

	private:
		void startFileWatcher();

	private:
		CPath mRootDirectory;
		FileSystemWatcher mFileWatcher;
	};
}

#endif
