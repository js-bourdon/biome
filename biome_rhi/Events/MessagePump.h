#pragma once

namespace biome::rhi
{
    namespace events
    {
        typedef void (*MessageConsumerCallback)(void* const pContext, const void* const pMsg);

        struct MessageConsumer
        {
            MessageConsumerCallback m_Callback {};
            void* m_pContext { nullptr };
        };

        bool PumpMessages();
        void RegisterMessageConsumer(const MessageConsumer& consumer);
    }
}
