#pragma once

namespace biome::rhi
{
    namespace descriptors
    {
        enum class ComparisonFunction
        {
            NEVER,
            LESS,
            EQUAL,
            LESS_EQUAL,
            GREATER,
            NOT_EQUAL,
            GREATER_EQUAL,
            ALWAYS,
            EnumCount
        };

        enum class StencilOperation
        {
            KEEP,
            ZERO,
            REPLACE,
            INCR_SAT,
            DECR_SAT,
            INVERT,
            INCR,
            DECR,
            EnumCount
        };

        struct StencilOperationDesc
        {
            StencilOperation StencilFailOp;
            StencilOperation DepthFailOp;
            StencilOperation DepthStencilPassOp;
            ComparisonFunction Function;
        };

        struct DepthStencilStateDesc
        {
            StencilOperationDesc FrontFaceStencil {};
            StencilOperationDesc BackFaceStencil {};
            ComparisonFunction DepthFunction {};
            uint8_t StencilReadMask {};
            uint8_t StencilWriteMask {};
            bool IsDepthTestEnabled {};
            bool IsDepthWriteEnabled {};
            bool IsStencilEnabled {};
        };
    }
}