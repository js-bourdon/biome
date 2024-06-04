#include <pch.h>
#include "MessagePump.h"

#include "biome_core/DataStructures/Vector.h"

static biome::data::Vector<biome::rhi::events::MessageConsumer> s_Consumers = {};

bool biome::rhi::events::PumpMessages()
{
    MSG msg;
    msg.message = WM_NULL;

    if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        for (biome::rhi::events::MessageConsumer consumer : s_Consumers)
        {
            (*consumer.m_Callback)(consumer.m_pContext, &msg);
        }
    }

    return msg.message == WM_QUIT;
}

void biome::rhi::events::RegisterMessageConsumer(const MessageConsumer& consumer)
{
    s_Consumers.Add(consumer);
}
