#pragma once

namespace biome
{
    namespace threading
    {
        class WorkerTask
        {
        public:

            // These will be called from a worker thread.
            virtual void DoWork() = 0;
            virtual void OnWorkDone() = 0;
        };
    }
}
