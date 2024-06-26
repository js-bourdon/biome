#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <type_traits>
#include <tuple>

#include "biome_core/Memory/ThreadHeapAllocator.h"

namespace biome
{
    namespace threading
    {
        template<typename R, typename ...A>
        class WorkerThread;

        // Worker thread running a function any number of times, yielding in between runs.
        // This allows avoiding to create/delete threads every time a threaded function
        // has to execute.
        //
        // The worker thread is created and then yields/waits/sleeps until another thread
        // calls `Run` with the required arguments. The worker thread then wakes, 
        // executes the function, notify any waiting thread and yields/sleeps/waits again.
        //
        template<typename ReturnType, typename ...ArgumentTypes>
        class WorkerThread<ReturnType(ArgumentTypes...)>
        {
        public:

            typedef ReturnType(*FunctionType)(ArgumentTypes...);

            WorkerThread(FunctionType threadFunction, size_t heapByteSize, size_t initialCommitByteSize);
            ~WorkerThread();

            void Init();
            void Shutdown();

            void Run(ArgumentTypes... args);
            ReturnType Wait();

        private:

            static void ThreadMain(WorkerThread *thisThread, size_t heapByteSize, size_t initialCommitByteSize);

            void Execute();

            FunctionType m_Function;
            ReturnType m_ReturnedArg {};
            std::tuple<ArgumentTypes...> m_Arguments;

            std::thread m_Thread;
            std::mutex m_Mutex {};
            std::condition_variable m_CondValue {};

            bool m_Running { false };
        };
    }
}

using namespace biome::threading;
using namespace biome::memory;

template<typename ReturnType, typename ...ArgumentTypes>
WorkerThread<ReturnType(ArgumentTypes...)>::WorkerThread(FunctionType threadFunction, size_t heapByteSize, size_t initialCommitByteSize)
    : m_Function(threadFunction)
    , m_Thread(ThreadMain, this, heapByteSize, initialCommitByteSize)
{ 

}

template<typename ReturnType, typename ...ArgumentTypes>
WorkerThread<ReturnType(ArgumentTypes...)>::~WorkerThread()
{
    Shutdown();
}

template<typename ReturnType, typename ...ArgumentTypes>
void WorkerThread<ReturnType(ArgumentTypes...)>::Init()
{
    std::unique_lock<std::mutex> lck(m_Mutex);

    if (!m_Running)
    {
        m_CondValue.wait(lck);
    }
}

template<typename ReturnType, typename ...ArgumentTypes>
void WorkerThread<ReturnType(ArgumentTypes...)>::Shutdown()
{
    m_Running = false;
    m_CondValue.notify_all();
    m_Thread.join();
}

template<typename ReturnType, typename ...ArgumentTypes>
void WorkerThread<ReturnType(ArgumentTypes...)>::Run(ArgumentTypes... args)
{
    std::unique_lock<std::mutex> lck(m_Mutex);
    m_Arguments = std::make_tuple(args...);
    m_CondValue.notify_all();
}

template<typename ReturnType, typename ...ArgumentTypes>
ReturnType WorkerThread<ReturnType(ArgumentTypes...)>::Wait()
{
    std::unique_lock<std::mutex> lck(m_Mutex);
    return m_ReturnedArg;
}

template<typename ReturnType, typename ...ArgumentTypes>
void WorkerThread<ReturnType(ArgumentTypes...)>::ThreadMain(WorkerThread *thisThread, size_t heapByteSize, size_t initialCommitByteSize)
{
    BIOME_ASSERT_ALWAYS_EXEC(ThreadHeapAllocator::Initialize(heapByteSize, initialCommitByteSize));
    thisThread->Execute();
}

template<typename ReturnType, typename ...ArgumentTypes>
void WorkerThread<ReturnType(ArgumentTypes...)>::Execute()
{
    std::unique_lock<std::mutex> lck(m_Mutex);

    m_Running = true;
    m_CondValue.notify_all();
    m_CondValue.wait(lck);

    while (m_Running)
    {
        m_ReturnedArg = std::apply(m_Function, m_Arguments);
        m_CondValue.wait(lck);
    }

    ThreadHeapAllocator::Shutdown();
}