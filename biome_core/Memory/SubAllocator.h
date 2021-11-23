#pragma once

namespace biome
{
    namespace memory
    {
        class SubAllocator
        {
        public:

            SubAllocator() = default;
            ~SubAllocator();

            template<typename T>
            void Accumulate(size_t count)
            {
                m_accumulatedBytes = memory::Align(m_accumulatedBytes, alignof(T)) + (sizeof(T) * count);
            }

        private:

            uint8_t*    m_pAllocation { nullptr };
            size_t      m_accumulatedBytes { 0 };
        };
    }
}