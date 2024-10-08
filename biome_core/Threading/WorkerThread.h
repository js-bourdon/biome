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
        class WorkerThread<ReturnType(ArgumentTypes...) noexcept>
        {
        public:

            static constexpr ReturnType(*fnctPtr)(ArgumentTypes...) noexcept = nullptr;
            static constexpr void(*callbackPtr)(const WorkerThread*, ReturnType) noexcept = nullptr;
            using FunctionPtrType = std::remove_const_t<decltype(fnctPtr)>;
            using CallbackPtrType = std::remove_const_t<decltype(callbackPtr)>;

            WorkerThread(FunctionPtrType threadFunction, size_t heapByteSize, size_t initialCommitByteSize);
            ~WorkerThread();

            WorkerThread(const WorkerThread&) = delete;
            WorkerThread& operator=(const WorkerThread&) = delete;

            void Init();
            void Shutdown();

            void Run(ArgumentTypes... args);
            void Run(CallbackPtrType callbackFunction, ArgumentTypes... args);
            ReturnType Wait();

        private:

            static void ThreadMain(WorkerThread *thisThread, size_t heapByteSize, size_t initialCommitByteSize);

            void Execute();

            FunctionPtrType m_Function;
            CallbackPtrType m_Callback { nullptr };
            ReturnType m_ReturnedArg {};
            std::tuple<ArgumentTypes...> m_Arguments;

            std::thread m_Thread;
            std::mutex m_Mutex {};
            std::condition_variable m_CondValue {};

            uint64_t m_RunIndex { 0 };
            uint64_t m_NextRunIndex { 0 };
            bool m_Executing { false };
        };
    }
}

using namespace biome::threading;
using namespace biome::memory;

template<typename ReturnType, typename ...ArgumentTypes>
WorkerThread<ReturnType(ArgumentTypes...) noexcept>::WorkerThread(FunctionPtrType threadFunction, size_t heapByteSize, size_t initialCommitByteSize)
    : m_Function(threadFunction)
    , m_Thread(ThreadMain, this, heapByteSize, initialCommitByteSize)
{ 

}

template<typename ReturnType, typename ...ArgumentTypes>
WorkerThread<ReturnType(ArgumentTypes...) noexcept>::~WorkerThread()
{
    Shutdown();
}

template<typename ReturnType, typename ...ArgumentTypes>
void WorkerThread<ReturnType(ArgumentTypes...) noexcept>::Init()
{
    std::unique_lock<std::mutex> lck(m_Mutex);
    m_CondValue.wait(lck, [&]{ return m_Executing; });
}

template<typename ReturnType, typename ...ArgumentTypes>
void WorkerThread<ReturnType(ArgumentTypes...) noexcept>::Shutdown()
{
    {
        std::lock_guard<std::mutex> lck(m_Mutex);
        m_Executing = false;
        ++m_NextRunIndex;
    }

    m_CondValue.notify_all();

    if (m_Thread.joinable())
    {
        m_Thread.join();
    }
}

template<typename ReturnType, typename ...ArgumentTypes>
void WorkerThread<ReturnType(ArgumentTypes...) noexcept>::Run(ArgumentTypes... args)
{
    Run(nullptr, std::forward<ArgumentTypes>(args)...);
}

template<typename ReturnType, typename ...ArgumentTypes>
void WorkerThread<ReturnType(ArgumentTypes...) noexcept>::Run(CallbackPtrType callbackFunction, ArgumentTypes... args)
{
    {
        std::lock_guard<std::mutex> lck(m_Mutex);
        m_Arguments = std::make_tuple(args...);
        m_Callback = callbackFunction;
        ++m_NextRunIndex;
    }
    
    m_CondValue.notify_all();
}

template<typename ReturnType, typename ...ArgumentTypes>
ReturnType WorkerThread<ReturnType(ArgumentTypes...) noexcept>::Wait()
{
    std::unique_lock<std::mutex> lck(m_Mutex);
    m_CondValue.wait(lck, [&] { return m_RunIndex == m_NextRunIndex; });
    return m_ReturnedArg;
}

template<typename ReturnType, typename ...ArgumentTypes>
void WorkerThread<ReturnType(ArgumentTypes...) noexcept>::ThreadMain(WorkerThread *thisThread, size_t heapByteSize, size_t initialCommitByteSize)
{
    BIOME_ASSERT_ALWAYS_EXEC(ThreadHeapAllocator::Initialize(heapByteSize, initialCommitByteSize));
    thisThread->Execute();
}

template<typename ReturnType, typename ...ArgumentTypes>
void WorkerThread<ReturnType(ArgumentTypes...) noexcept>::Execute()
{
    {
        std::lock_guard<std::mutex> execLock(m_Mutex);
        m_Executing = true;
    }
    
    m_CondValue.notify_all();

    std::unique_lock<std::mutex> lck(m_Mutex);
    m_CondValue.wait(lck, [&] { return m_RunIndex < m_NextRunIndex; });

    while (m_Executing)
    {
        m_ReturnedArg = std::apply(m_Function, m_Arguments);

        if (m_Callback) 
        {
            (*m_Callback)(this, m_ReturnedArg);
        }

        m_RunIndex = m_NextRunIndex;

        m_CondValue.notify_all();
        m_CondValue.wait(lck, [&] { return m_RunIndex < m_NextRunIndex; });
    }

    ThreadHeapAllocator::Shutdown();
}