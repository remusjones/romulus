#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/vec4.hpp>
#include "VulkanGraphicsImpl.h"
#include "spdlog/spdlog.h"

#include <new>
#include <cstdint>

#include "Memory.h"

void* __cdecl operator new(size_t size) { return Memory::AllocImpl(size); }
void* __cdecl operator new[](size_t size) { return Memory::AllocImpl(size); }

void __cdecl operator delete(void* ptr) noexcept { Memory::FreeImpl(ptr); }
void __cdecl operator delete[](void* ptr) noexcept { Memory::FreeImpl(ptr); }

void __cdecl operator delete(void* ptr, size_t size) noexcept { Memory::FreeImpl(ptr); }
void __cdecl operator delete[](void* ptr, size_t size) noexcept { Memory::FreeImpl(ptr); }


void* __cdecl operator new[](size_t size, const char* name, int flags,
                         unsigned debugFlags, const char* file, int line)
{
    return Memory::AllocImpl(size, name);
}

void* __cdecl operator new[](unsigned __int64 size, unsigned __int64 alignment,
                             unsigned __int64 offset, char const* pName,
                             int flags, unsigned int debugFlags,
                             char const* file, int line)
{
    return Memory::AllocImpl(size, pName);
}

int main()
{
    SPDLOG_INFO("Initializing Application");
    eastl::unique_ptr<IApplication> app = eastl::make_unique<VulkanGraphicsImpl>("Engine", 1200, 800);
    try
    {
        app->Run();
    }
    catch (const std::exception& error)
    {
        SPDLOG_CRITICAL(error.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
