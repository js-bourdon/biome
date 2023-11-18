#include <pch.h>
#include <direct.h>
#include "FileSystemWatcher.h"

using namespace biome::filesystem;
using namespace biome::memory;

DWORD WINAPI FileSystemWatcher::ThreadProc(void* pContext)
{
	FileSystemWatcher* pWatcher = static_cast<FileSystemWatcher*>(pContext);
	pWatcher->ProcessNotifications();
	return 0;
}

bool FileSystemWatcher::Initialize(FileSystemWatcherCallback callback)
{
	m_callback = callback;
	m_thread = CreateThread(nullptr, 0, ThreadProc, this, 0, nullptr);
	return m_thread != NULL;
}

FileSystemWatcher::~FileSystemWatcher()
{

}

bool FileSystemWatcher::WatchDirectory(const wchar_t* directoryPath)
{
	std::unique_lock<std::mutex> lck(m_mutex);

	std::wstring filePath(directoryPath);

	if (m_watchedDirectories.find(filePath) != m_watchedDirectories.end())
	{
		return true;
	}

	HANDLE dirHdl = 
		CreateFileW(
			directoryPath, 
			FILE_LIST_DIRECTORY, 
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 
			nullptr, 
			OPEN_EXISTING, 
			FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);

	if (dirHdl == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	HANDLE eventHdl = CreateEventW(nullptr, FALSE, FALSE, directoryPath);

	if (eventHdl == NULL)
	{
		CloseHandle(dirHdl);
		return false;
	}

	auto pair = m_watchedDirectories.emplace(std::make_pair(std::move(filePath), WatchedDirectoryData {}));
	WatchedDirectoryData& data = pair.first->second;

	data.m_overlapped.hEvent = eventHdl;

	BOOL operationQueued = ReadDirectoryChangesW(
		dirHdl,
		data.m_notificationBuffer,
		ARRAYSIZE(data.m_notificationBuffer),
		TRUE,
		FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE,
		nullptr,
		&data.m_overlapped,
		nullptr);

	if (!operationQueued)
	{
		CloseHandle(dirHdl);
		CloseHandle(eventHdl);
		m_watchedDirectories.erase(pair.first);

		return false;
	}

	return true;
}

void FileSystemWatcher::StopWatchingDirectory(const wchar_t* directoryPath)
{
	std::unique_lock<std::mutex> lck(m_mutex);
}

void FileSystemWatcher::StopAllWatching()
{
	std::unique_lock<std::mutex> lck(m_mutex);
}

void FileSystemWatcher::ProcessNotifications()
{
	std::unique_lock<std::mutex> lck(m_mutex);

	while (m_isRunning)
	{


		for (auto& pair : m_watchedDirectories)
		{
			const WatchedDirectoryData& data = pair.second;
		}




		m_conditionVar.wait(lck);
	}
}
