#pragma once

#include <map>
#include <string>
#include <mutex>
#include <condition_variable>

namespace biome
{
    namespace filesystem
    {
        typedef void (*FileSystemWatcherCallback)();

        class FileSystemWatcher
        {
        public:

            FileSystemWatcher() = default;
            ~FileSystemWatcher();
            FileSystemWatcher(const FileSystemWatcher&) = delete;
            FileSystemWatcher(FileSystemWatcher&&) = delete;

            bool Initialize(FileSystemWatcherCallback callback);
            bool WatchDirectory(const wchar_t* directoryPath);
            void StopWatchingDirectory(const wchar_t* directoryPath);
            void StopAllWatching();

        private:

            void ProcessNotifications();
            static DWORD WINAPI ThreadProc(void* pContext);

            typedef uint8_t NotificationBuffer[1024];

            struct WatchedDirectoryData
            {
                OVERLAPPED m_overlapped { 0 };
                NotificationBuffer m_notificationBuffer {};
            };

            FileSystemWatcherCallback m_callback { nullptr };
            HANDLE m_thread { 0 };
            std::mutex m_mutex {};
            std::condition_variable m_conditionVar {};


            std::map<std::wstring, WatchedDirectoryData> m_watchedDirectories {};


            bool m_isRunning { false };
        };
    }
}
