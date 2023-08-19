#ifndef COCOA_ENGINE_FILE_SYSTEM_WATCHER_H
#define COCOA_ENGINE_FILE_SYSTEM_WATCHER_H

#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include <filesystem>

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
		~FileSystemWatcher() { stop(); }
		void start();
		void stop();

	public:
		typedef void (*OnChanged)(const std::filesystem::path& file);
		typedef void (*OnRenamed)(const std::filesystem::path& file);
		typedef void (*OnDeleted)(const std::filesystem::path& file);
		typedef void (*OnCreated)(const std::filesystem::path& file);

		OnChanged onChanged = nullptr;
		OnRenamed onRenamed = nullptr;
		OnDeleted onDeleted = nullptr;
		OnCreated onCreated = nullptr;

		int notifyFilters = 0;
		bool includeSubdirectories = false;
		std::string filter = "";
		std::filesystem::path path = {};

	private:
		void startThread();

	private:
		bool mEnableRaisingEvents = true;
		std::thread mThread;

#ifdef _WIN32
		HANDLE mHStopEvent;
#endif
	};
}

#endif
