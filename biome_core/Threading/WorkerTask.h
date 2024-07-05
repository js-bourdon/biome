#pragma once

namespace biome
{
    namespace threading
    {
        class WorkerTask
        {
        public:

            // These will be called from a worker thread.
            virtual void DoWork() noexcept = 0;
            virtual void OnWorkDone() noexcept = 0;
        };
    }
}
