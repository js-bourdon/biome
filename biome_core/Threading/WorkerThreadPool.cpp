#include <pch.h>
#include "biome_core/Threading/WorkerThreadPool.h"
#include "biome_core/Threading/WorkerTask.h"

using namespace biome::threading;

WorkerThreadPool::WorkerThreadPool(const uint32_t threadCount, size_t perThreadHeapByteSize, size_t perThreadInitialCommitByteSize)
    : m_AvailableWorkers(threadCount)
    , m_Workers(threadCount, this, DoWork, perThreadHeapByteSize, perThreadInitialCommitByteSize)
    , m_TaskQueue(threadCount)
    , m_Thread(DispatchTasks, this, perThreadHeapByteSize, perThreadInitialCommitByteSize)
{
    for (Worker& worker : m_Workers)
    {
        m_AvailableWorkers.Add(&worker);
    }
}

WorkerThreadPool::~WorkerThreadPool()
{
    m_isRunning = false;
    m_CondValue.notify_all();
    m_Thread.join();
}

void WorkerThreadPool::QueueTask(WorkerTask* const pTask)
{
    {
        std::lock_guard<std::mutex> lck(m_Mutex);
        m_TaskQueue.Add(pTask);
    }

    m_CondValue.notify_all();
}

WorkerThreadPool::Worker* WorkerThreadPool::DoWork(Worker* pWorker)
{
    pWorker->m_pTask->DoWork();
    return pWorker;
}

void WorkerThreadPool::WorkDone(const WorkerThread<WorkerFnctType>* pThread, Worker* pWorker)
{
    pWorker->m_pThreadPool->OnWorkDone(pWorker);
}

void WorkerThreadPool::OnWorkDone(Worker* const pWorker)
{
    pWorker->m_pTask->OnWorkDone();

    {
        std::lock_guard<std::mutex> lck(m_Mutex);
        m_AvailableWorkers.Add(pWorker);
    }

    m_CondValue.notify_all();
}

void WorkerThreadPool::DispatchTasks(WorkerThreadPool* pThreadPool, size_t heapByteSize, size_t initialCommitByteSize)
{
    BIOME_ASSERT_ALWAYS_EXEC(ThreadHeapAllocator::Initialize(heapByteSize, initialCommitByteSize));
    pThreadPool->m_isRunning = true;
    pThreadPool->OnDispatchTasks();
}

void WorkerThreadPool::OnDispatchTasks()
{
    while (m_isRunning)
    {
        std::unique_lock<std::mutex> lck(m_Mutex);
        while (m_TaskQueue.Size() > 0 && m_AvailableWorkers.Size() > 0)
        {
            WorkerTask* pTask = m_TaskQueue.PopBack();
            Worker* pWorker = m_AvailableWorkers.PopBack();
            pWorker->m_pTask = pTask;
            pWorker->m_WorkerThread.Run(WorkDone, pWorker);
        }

        m_CondValue.wait(lck);
    }
}
