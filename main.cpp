#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include "VulkanGraphicsImpl.h"
#include "spdlog/spdlog.h"

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
