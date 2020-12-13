#pragma once
#include "externalLibs.h"
#include "jade/core/Core.h"

#include "jade/file/FileSystemWatcher.h"

namespace Jade
{
	class SourceFileWatcher
	{
	public:
		SourceFileWatcher() 
			: m_RootDirectory("") {}
		SourceFileWatcher(JPath path);
		~SourceFileWatcher() { m_FileWatcher.Stop(); }

	private:
		void StartFileWatcher();

	private:
		JPath m_RootDirectory;
		FileSystemWatcher m_FileWatcher;
	};
}