cmake_minimum_required(VERSION 3.20)
project(Romulus VERSION 1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

if(MSVC)
    add_compile_definitions(_AMD64_)
endif()

find_package(Vulkan REQUIRED)

file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
        "src/*.cpp"
        "src/*.h"
)

add_library(Romulus_Lib STATIC ${ENGINE_SOURCES})

add_subdirectory(vendor)
add_subdirectory(Assets)

target_compile_definitions(Romulus_Lib PUBLIC
        GLM_ENABLE_EXPERIMENTAL
        TRACY_ENABLE=1
        TRACY_MEMORY_ENABLE=1
)

target_include_directories(Romulus_Lib PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Graphics/API>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Applications>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/IO>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Common>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/System>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb>
)

target_link_libraries(Romulus_Lib PUBLIC
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
        EASTL
        Tracy::TracyClient
)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE Romulus_Lib)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_options(Romulus_Lib INTERFACE "-static-libgcc" "-static-libstdc++")
endif()

# copy the assets to the bin folder and compile binaries
copy_assets(${PROJECT_NAME} "Assets/Models" "Models")
copy_assets(${PROJECT_NAME} "Assets/Textures" "Textures")
compile_shaders(${PROJECT_NAME} "Assets/Shaders")
