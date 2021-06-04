#pragma once
#include "externalLibs.h"
#include "cocoa/core/Core.h"
#include "cocoa/file/FileSystemWatcher.h"

namespace Cocoa
{
	class SourceFileWatcher
	{
	public:
		SourceFileWatcher() 
			: m_RootDirectory(CPath::Create()) {}
		SourceFileWatcher(CPath path);
		~SourceFileWatcher() { m_FileWatcher.Stop(); }

	private:
		void StartFileWatcher();

	private:
		CPath m_RootDirectory;
		FileSystemWatcher m_FileWatcher;
	};
}