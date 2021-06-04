#include "cocoa/file/FileSystemWatcher.h"

namespace Cocoa
{
	FileSystemWatcher::FileSystemWatcher()
	{

	}

	void FileSystemWatcher::Start()
	{
		m_Thread = std::thread(&FileSystemWatcher::StartThread, this);
	}

#ifdef _WIN32
	void FileSystemWatcher::StartThread()
	{
		if (m_Path.Size() == 0)
		{
			return;
		}

		HANDLE dirHandle = CreateFileA(m_Path.Path, GENERIC_READ | FILE_LIST_DIRECTORY,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
			NULL);
		if (dirHandle == INVALID_HANDLE_VALUE)
		{
			Logger::Error("Invalid file access. Could not create FileSystemWatcher for '%s'", m_Path.Path);
			return;
		}

		// Set up notification flags 
		int flags = 0;
		if (m_NotifyFilters && NotifyFilters::FileName)
		{
			flags |= FILE_NOTIFY_CHANGE_FILE_NAME;
		}

		if (m_NotifyFilters && NotifyFilters::DirectoryName)
		{
			flags |= FILE_NOTIFY_CHANGE_DIR_NAME;
		}

		if (m_NotifyFilters && NotifyFilters::Attributes)
		{
			flags |= FILE_NOTIFY_CHANGE_ATTRIBUTES;
		}

		if (m_NotifyFilters && NotifyFilters::Size)
		{
			flags |= FILE_NOTIFY_CHANGE_SIZE;
		}

		if (m_NotifyFilters && NotifyFilters::LastWrite)
		{
			flags |= FILE_NOTIFY_CHANGE_LAST_WRITE;
		}

		if (m_NotifyFilters && NotifyFilters::LastAccess)
		{
			flags |= FILE_NOTIFY_CHANGE_LAST_ACCESS;
		}

		if (m_NotifyFilters && NotifyFilters::CreationTime)
		{
			flags |= FILE_NOTIFY_CHANGE_CREATION;
		}

		if (m_NotifyFilters && NotifyFilters::Security)
		{
			flags |= FILE_NOTIFY_CHANGE_SECURITY;
		}

		char filename[MAX_PATH];
		char buffer[2048];
		DWORD bytesReturned;
		FILE_NOTIFY_INFORMATION* pNotify;
		int offset = 0;
		OVERLAPPED pollingOverlap;
		pollingOverlap.OffsetHigh = 0;
		pollingOverlap.hEvent = CreateEventA(NULL, TRUE, FALSE, NULL);
		if (pollingOverlap.hEvent == NULL)
		{
			Logger::Error("Could not create event watcher for FileSystemWatcher '%s'", m_Path.Path);
			return;
		}

		bool result = true;
		HANDLE hEvents[2];
		hEvents[0] = pollingOverlap.hEvent;
		hEvents[1] = CreateEventA(NULL, TRUE, FALSE, NULL);
		hStopEvent = hEvents[1];

		while (result && m_EnableRaisingEvents)
		{
			result = ReadDirectoryChangesW(
				dirHandle,                   // handle to the directory to be watched
				&buffer,                     // pointer to the buffer to receive the read results
				sizeof(buffer),              // length of lpBuffer
				m_IncludeSubdirectories,     // flag for monitoring directory or directory tree
				flags,
				&bytesReturned,              // number of bytes returned
				&pollingOverlap,             // pointer to structure needed for overlapped I/O
				NULL
			);

			DWORD event = WaitForMultipleObjects(2, hEvents, FALSE, INFINITE);
			offset = 0;
			int rename = 0;

			if (event == WAIT_OBJECT_0 + 1)
			{
				break;
			}

			do
			{
				pNotify = (FILE_NOTIFY_INFORMATION*)((char*)buffer + offset);
				strcpy(filename, "");
				int filenamelen = WideCharToMultiByte(CP_ACP, 0, pNotify->FileName, pNotify->FileNameLength / 2, filename, sizeof(filename), NULL, NULL);
				filename[pNotify->FileNameLength / 2] = '\0';
				switch (pNotify->Action)
				{
				case FILE_ACTION_ADDED:
					if (m_OnCreated != nullptr)
					{
						m_OnCreated(CPath::Create(filename));
					}
					break;
				case FILE_ACTION_REMOVED:
					if (m_OnDeleted != nullptr)
					{
						m_OnDeleted(CPath::Create(filename));
					}
					break;
				case FILE_ACTION_MODIFIED:
					if (m_OnChanged != nullptr)
					{
						m_OnChanged(CPath::Create(filename));
					}
					break;
				case FILE_ACTION_RENAMED_OLD_NAME:
					// Logger::Info("The file was renamed and this is the old name: [%s]", filename);
					break;
				case FILE_ACTION_RENAMED_NEW_NAME:
					if (m_OnRenamed != nullptr)
					{
						m_OnRenamed(CPath::Create(filename));
					}
					break;
				default:
					Logger::Error("Default error. Unknown file action '%d' for FileSystemWatcher '%s'", pNotify->Action, m_Path.Path);
					break;
				}

				offset += pNotify->NextEntryOffset;
			} while (pNotify->NextEntryOffset);


		}

		CloseHandle(dirHandle);
	}

	void FileSystemWatcher::Stop()
	{
		if (m_EnableRaisingEvents)
		{
			m_EnableRaisingEvents = false;
			SetEvent(hStopEvent);
			m_Thread.join();
		}
	}
#endif
}