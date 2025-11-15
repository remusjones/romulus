cmake_minimum_required(VERSION 3.20)
project(Romulus VERSION 1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)

file(GLOB_RECURSE PROJECT_SOURCE_FILES CONFIGURE_DEPENDS
        "Engine/*.cpp"
        "Engine/*.h"
)

# Define the executable target
add_executable(
        ${PROJECT_NAME}
        main.cpp
        ${PROJECT_SOURCE_FILES}
)

add_subdirectory(vendor)
add_subdirectory(Assets)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        GLM_ENABLE_EXPERIMENTAL
)

target_include_directories(${PROJECT_NAME} PRIVATE
        Engine/Graphics/API
        Engine/Applications
        Engine/IO
        Engine/Common
        Engine/System
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        Vulkan::Vulkan
        SDL3::SDL3-static
        imgui
        imguizmo
        spdlog
        glm
        tinyobjloader
        GPUOpen::VulkanMemoryAllocator
        BulletSoftBody
        BulletDynamics
        BulletCollision
        LinearMath
)

# Add static libc++ options only for GNU toolchains (e.g., MinGW)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_options(${PROJECT_NAME} PRIVATE
            "-static-libgcc"
            "-static-libstdc++"
    )
endif()