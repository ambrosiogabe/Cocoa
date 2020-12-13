#pragma once

#include "externalLibs.h"
#include "jade/core/Core.h"
#include "JPath.h"

#include <thread>
#ifdef _WIN32
#include <Windows.h>
#endif

namespace Jade
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

	class JADE FileSystemWatcher
	{
	public:
		FileSystemWatcher();
		~FileSystemWatcher() {}
		void Start();
		void Stop();

	public:
		typedef void (*OnChanged)(const JPath& file);
		typedef void (*OnRenamed)(const JPath& file);
		typedef void (*OnDeleted)(const JPath& file);
		typedef void (*OnCreated)(const JPath& file);

		OnChanged m_OnChanged = nullptr;
		OnRenamed m_OnRenamed = nullptr;
		OnDeleted m_OnDeleted = nullptr;
		OnCreated m_OnCreated = nullptr;

		int m_NotifyFilters = 0;
		bool m_IncludeSubdirectories = false;
		std::string m_Filter = "";
		JPath m_Path = "";

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