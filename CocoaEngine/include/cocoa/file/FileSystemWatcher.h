#pragma once

#include "externalLibs.h"
#include "cocoa/core/Core.h"
#include "CPath.h"

#include <thread>
#ifdef _WIN32
#include <windows.h>
#endif

#undef CopyFile
#undef DeleteFile
#undef CreateFile

namespace Cocoa
{
	enum NotifyFilters
	{
		FileName = 1,
		DirectoryName = 2,
		Attributes = 4,
		Size = 8,
		LastWrite = 16,
		LastAccess = 32,
		CreationTime = 64,
		Security = 256
	};

	class COCOA FileSystemWatcher
	{
	public:
		FileSystemWatcher();
		~FileSystemWatcher() { Stop(); }
		void Start();
		void Stop();

	public:
		typedef void (*OnChanged)(const CPath& file);
		typedef void (*OnRenamed)(const CPath& file);
		typedef void (*OnDeleted)(const CPath& file);
		typedef void (*OnCreated)(const CPath& file);

		OnChanged m_OnChanged = nullptr;
		OnRenamed m_OnRenamed = nullptr;
		OnDeleted m_OnDeleted = nullptr;
		OnCreated m_OnCreated = nullptr;

		int m_NotifyFilters = 0;
		bool m_IncludeSubdirectories = false;
		std::string m_Filter = "";
		CPath m_Path = NCPath::CreatePath();

	private:
		bool m_EnableRaisingEvents = true;
		std::thread m_Thread;

#ifdef _WIN32
		HANDLE hStopEvent;
#endif

	private:
		void StartThread();
	};
}