#pragma once

#include "biome_core/Threading/WorkerThread.h"
#include "biome_core/DataStructures/Vector.h"
#include "biome_core/DataStructures/StaticArray.h"

namespace biome
{
    namespace threading
    {
        class WorkerTask;

        class WorkerThreadPool
        {
        public:

            WorkerThreadPool(const uint32_t threadCount, size_t perThreadHeapByteSize, size_t perThreadInitialCommitByteSize);
            ~WorkerThreadPool();

            void QueueTask(WorkerTask* const pTask);

        private:

            struct Worker;
            typedef Worker*(WorkerFnctType)(Worker*);

            struct Worker
            {
                Worker(WorkerThreadPool* pThreadPool, WorkerFnctType workerFnct, size_t heapByteSize, size_t initialCommitByteSize)
                    : m_pThreadPool(pThreadPool)
                    , m_WorkerThread(workerFnct, heapByteSize, initialCommitByteSize)
                    , m_pTask(nullptr) {}

                WorkerThreadPool* m_pThreadPool;
                WorkerThread<WorkerFnctType> m_WorkerThread;
                WorkerTask* m_pTask;
            };

            static Worker* DoWork(Worker* worker);
            static void WorkDone(const WorkerThread<WorkerFnctType>* pThread, Worker* pWorker);
            static void DispatchTasks(WorkerThreadPool* pThreadPool, size_t perThreadHeapByteSize, size_t perThreadInitialCommitByteSize);

            void OnWorkDone(Worker* const pWorker);
            void OnDispatchTasks();

            biome::data::Vector<Worker*> m_AvailableWorkers;
            biome::data::StaticArray<Worker> m_Workers;
            biome::data::Vector<WorkerTask*> m_TaskQueue;
            std::condition_variable m_CondValue {};
            std::mutex m_Mutex {};
            bool m_isRunning { false };
            std::thread m_Thread;
        };
    }
}
