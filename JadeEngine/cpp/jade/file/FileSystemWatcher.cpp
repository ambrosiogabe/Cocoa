#include "jade/file/FileSystemWatcher.h"
#include "jade/util/Log.h"

#ifdef _WIN32
#include <Windows.h>
#endif

namespace Jade
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
		if (m_Path == nullptr || m_Path.Size() == 0)
		{
			return;
		}

		HANDLE dirHandle = CreateFileA(m_Path.Filepath(), GENERIC_READ | FILE_LIST_DIRECTORY,
										FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
										NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
										NULL);
		if (dirHandle == INVALID_HANDLE_VALUE)
		{
			Log::Error("Invalid file access. Could not create FileSystemWatcher for '%s'", m_Path.Filepath());
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

		{
			pollingOverlap.OffsetHigh = 0;
			pollingOverlap.hEvent = CreateEventA(NULL, TRUE, FALSE, NULL);
		}

		bool result = true;

		while (result && m_EnableRaisingEvents)
		{
			Log::Info("Waiting for notification");

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

			WaitForSingleObject(pollingOverlap.hEvent, INFINITE);
			offset = 0;
			int rename = 0;
			char oldName[MAX_PATH];
			char newName[MAX_PATH];

			do
			{
				pNotify = (FILE_NOTIFY_INFORMATION*)((char*)buffer + offset);
				strcpy(filename, "");
				int filenamelen = WideCharToMultiByte(CP_ACP, 0, pNotify->FileName, pNotify->FileNameLength / 2, filename, sizeof(filename), NULL, NULL);
				filename[pNotify->FileNameLength / 2] = '\0';
				switch (pNotify->Action)
				{
				case FILE_ACTION_ADDED:
					Log::Info("The file is added to the directory: [%s]", filename);
					if (m_OnCreated != nullptr)
					{
						m_OnCreated(JPath(filename));
					}
					break;
				case FILE_ACTION_REMOVED:
					Log::Info("The file is removed from the directory: [%s]", filename);
					if (m_OnDeleted != nullptr)
					{
						m_OnDeleted(JPath(filename));
					}
					break;
				case FILE_ACTION_MODIFIED:
					Log::Info("The file is modified. This can be a change in the time stamp or attributes: [%s]", filename);
					if (m_OnChanged != nullptr)
					{
						m_OnChanged(JPath(filename));
					}
					break;
				case FILE_ACTION_RENAMED_OLD_NAME:
					Log::Info("The file was renamed and this is the old name: [%s]", filename);
					break;
				case FILE_ACTION_RENAMED_NEW_NAME:
					Log::Info("The file was renamed and this is the new name: [%s]", filename);
					if (m_OnRenamed != nullptr)
					{
						m_OnRenamed(JPath(filename));
					}
					break;
				default:
					Log::Error("Default error.");
					break;
				}

				offset += pNotify->NextEntryOffset;
			} while (pNotify->NextEntryOffset);

			
		}

		CloseHandle(dirHandle);
	}
#endif

	void FileSystemWatcher::Stop()
	{
		m_EnableRaisingEvents = false;
		m_Thread.join();
	}
}