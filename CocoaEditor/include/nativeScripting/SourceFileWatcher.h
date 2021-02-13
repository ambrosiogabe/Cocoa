#pragma once
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "nativeScripting/ScriptParser.h"

#include "cocoa/file/FileSystemWatcher.h"

namespace Cocoa
{
	class SourceFileWatcher
	{
	public:
		SourceFileWatcher() 
			: m_RootDirectory(NCPath::CreatePath()) {}
		SourceFileWatcher(CPath path);
		~SourceFileWatcher() { m_FileWatcher.Stop(); }

		static const std::vector<UClass>& GetClasses();

	private:
		void StartFileWatcher();

	private:
		CPath m_RootDirectory;
		FileSystemWatcher m_FileWatcher;
	};
}