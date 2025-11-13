cmake_minimum_required(VERSION 4.0)
project(Artemis VERSION 1.0)

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

include_directories(${Vulkan_INCLUDE_DIRS})

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
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        Vulkan::Vulkan
        SDL3-static
        imgui
        imguizmo
        spdlog
        glm
        tinyobjloader
        BulletSoftBody
        BulletDynamics
        BulletCollision
        LinearMath
)

target_link_options(${PROJECT_NAME} PRIVATE
        "-static-libgcc"
        "-static-libstdc++"
)