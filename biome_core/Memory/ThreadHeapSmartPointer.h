#pragma once

#include "biome_core/Memory/ThreadHeapAllocator.h"

namespace biome
{
    namespace memory
    {
        template<typename T>
        struct ThreadHeapSmartPointer
        {
            ThreadHeapSmartPointer(void *pPtr) : m_pPtr(static_cast<T*>(pPtr)) {}
            ThreadHeapSmartPointer(const ThreadHeapSmartPointer& other) = delete;
            ThreadHeapSmartPointer(ThreadHeapSmartPointer&& other) noexcept { m_pPtr = other.m_pPtr; other.m_pPtr = nullptr; }
            ~ThreadHeapSmartPointer() { if (m_pPtr) ThreadHeapAllocator::Release(m_pPtr); }
            ThreadHeapSmartPointer& operator=(ThreadHeapSmartPointer&& other) = delete;
            ThreadHeapSmartPointer& operator=(const ThreadHeapSmartPointer& other) = delete;

            inline operator T*() { return m_pPtr; }
            inline T& operator[](size_t index) { return m_pPtr[index]; }
            inline T* operator+(size_t offset) { return m_pPtr + offset; }
            inline T* operator->() { return m_pPtr; }
            inline T operator*() { return *m_pPtr; }

			inline operator const T* () const { return m_pPtr; }
			inline const T& operator[](size_t index) const { return m_pPtr[index]; }
			inline const T* operator+(size_t offset) const { return m_pPtr + offset; }
			inline const T* operator->() const { return m_pPtr; }
			inline const T operator*() const { return *m_pPtr; }

            inline T& operator=(const T& other) = delete;

        private:
            T* m_pPtr;
        };

        using str_smart_ptr = ThreadHeapSmartPointer<char>;
        using wstr_smart_ptr = ThreadHeapSmartPointer<wchar_t>;
    }
}