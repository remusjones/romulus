#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include "VulkanGraphicsImpl.h"
#include "spdlog/spdlog.h"

void* __cdecl operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line)
{
    return new uint8_t[size];
}

void* __cdecl operator new[](unsigned __int64 size, unsigned __int64 alignment, unsigned __int64 offset, char const* pName, int flags, unsigned int debugFlags, char const* file, int line)
{

    return new uint8_t[size];
}


int main()
{
    SPDLOG_INFO("Initializing Application");
    IApplication* app = new VulkanGraphicsImpl("Engine", 1200, 800);
    try
    {
        app->Run();
    }
    catch (const std::exception& error)
    {
        SPDLOG_CRITICAL(error.what());
        return EXIT_FAILURE;
    }

    delete app;
    return EXIT_SUCCESS;
}
