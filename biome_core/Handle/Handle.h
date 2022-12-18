#pragma once

#include <stdint.h>
#include <utility>
#include <limits>
#include "biome_core/Core/Defines.h"
#include "biome_core/Math/Math.h"

#define VALIDATE_HANDLES _DEBUG

namespace biome
{
    constexpr uintptr_t Handle_NULL = UINTPTR_MAX;

    struct alignas(sizeof(uintptr_t)) Handle
    {
        typedef uint8_t GenerationType;
        typedef uintptr_t ValueType;
        static constexpr size_t s_GenerationOffset = (sizeof(ValueType) - sizeof(GenerationType)) * 8;
        static constexpr size_t s_GenerationMask = std::numeric_limits<GenerationType>::max();
        static constexpr size_t s_ValueMask = ~(s_GenerationMask << s_GenerationOffset);

        Handle() : m_Value(UINTPTR_MAX) {}

        Handle(uintptr_t value) : m_Value(value) {}

        bool operator==(Handle other)
        {
            return m_Value == other.m_Value;
        }

        bool operator==(ValueType other)
        {
            return m_Value == other;
        }

#if VALIDATE_HANDLES
        operator ValueType() const
        {
            return GetValue();
        }

        GenerationType GetGeneration() const
        {
            return static_cast<GenerationType>((m_Value >> s_GenerationOffset) & s_GenerationMask);
        }

        void SetGeneration(GenerationType generation)
        {
            m_Value = static_cast<ValueType>(size_t(m_Value & s_ValueMask) | (size_t(generation) << s_GenerationOffset));
        }

        void IncrementGeneration()
        {
            GenerationType generation = GetGeneration();
            ++generation;
            SetGeneration(generation);
        }

        inline ValueType GetValue() const
        {
            return m_Value & s_ValueMask;
        }

        void SetValue(ValueType value)
        {
            BIOME_ASSERT_MSG(value == (value & s_ValueMask), "Handle::SetValue: value is too high to allow for storing generation in debug mode");
            m_Value = value | (m_Value & ~s_ValueMask);
        }
#else
        operator ValueType() const
        {
            return m_Value;
        }

        inline ValueType GetValue() const
        {
            return m_Value;
        }

        inline void SetValue(ValueType value)
        {
            m_Value = value;
        }
#endif

    private:

        ValueType m_Value { UINTPTR_MAX };
    };

    static_assert(sizeof(Handle) == sizeof(uintptr_t));

    /*
    typedef Handle::ValueType ResourceTypeMask;

    template<ResourceTypeMask TypeMask>
    struct ParentResourceHandle
    {
        static constexpr ResourceTypeMask s_TypeMask = TypeMask;
        static constexpr ResourceTypeMask s_SubTypeOffset = biome::math::CountLeadingZeros(TypeMask);
        static constexpr ResourceTypeMask s_SubTypeMask = TypeMask << s_SubTypeOffset;

        ParentResourceHandle() : m_Handle(Handle_NULL) {}
        ParentResourceHandle(uintptr_t value) : m_Handle(value) {}
        ParentResourceHandle(Handle handle) : m_Handle(handle) {}
            
        template<typename T>
        inline bool Is() const
        {
            return (((m_Handle.m_Value & s_SubTypeMask) >> s_SubTypeOffset) & T::s_TypeMask) == T::s_TypeMask; 
        }
            
        inline Handle TypeLessHandle() const
        {
            Handle newHdl(m_Handle);
            newHdl.m_Value = m_Handle.m_Value & ~s_SubTypeMask;
            return newHdl;
        }

        Handle m_Handle;
    };

    template<typename ParentType, ResourceTypeMask TypeMask,
        std::enable_if_t<std::is_base_of<ParentResourceHandle<ParentType::s_TypeMask>, ParentType>::value, int> = 0>
    struct ChildResourceHandle : ParentType
    {
        static constexpr ResourceTypeMask s_TypeMask = TypeMask;

        ChildResourceHandle() : ParentType(Handle_NULL) {}
        ChildResourceHandle(uintptr_t value) : ParentType(value) {}
        ChildResourceHandle(Handle handle) : ParentType(handle) {}
        operator ParentType() { return ParentType::m_Handle; }
            
        static void InsertType(Handle &hdl)
        {
            ParentType::m_Handle.SetValue(ParentType::m_Handle.GetValue() | ParentType::s_SubTypeMask);
        }
    };
    */
}