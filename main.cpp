#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include "Engine/System/Logger.h"
#include "VulkanGraphicsImpl.h"

int main()
{
    Logger::Log(spdlog::level::info, "Initializing Application");
    IApplication* app = new VulkanGraphicsImpl("Engine", 1200, 800);
    try
    {
        app->Run();
    }
    catch (const std::exception& error)
    {
        Logger::Log(spdlog::level::critical, error.what());
        return EXIT_FAILURE;
    }

    delete app;
    return EXIT_SUCCESS;
}
