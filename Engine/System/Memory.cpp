//
// Created by remus on 8/12/2025.
//

#include "Memory.h"

#include "tracy/Tracy.hpp"

/*static*/ void* Memory::AllocImpl(size_t size, const char* name)
{
    void* ptr = std::malloc(size);

    if (!ptr)
    {
        throw std::bad_alloc();
    }

#if TRACY_MEMORY_ENABLE
    if (isTracing)
    {
        if (name)
        {
            TracyAllocN(ptr, size, name);
        }
        else
        {
            TracyAlloc(ptr, size);
        }
    }
#endif

    return ptr;
}

/*static*/ void Memory::FreeImpl(void* ptr)
{
    if (!ptr)
    {
        return;
    }

#if TRACY_MEMORY_ENABLE
    if (isTracing)
    {
        TracyFree(ptr);
    }
#endif

    std::free(ptr);
}