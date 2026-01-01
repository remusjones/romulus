cmake_minimum_required(VERSION 3.20)
project(Sandbox)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

add_executable(${PROJECT_NAME} main.cpp
        SandboxGraphicsImpl.cpp
        SandboxGraphicsImpl.h
        SandboxScene.cpp
        SandboxScene.h)

add_subdirectory("../../" "${CMAKE_CURRENT_BINARY_DIR}/romulus_build")

target_link_libraries(Sandbox PRIVATE Romulus_Lib)

# Copy engine assets to the project build path
copy_assets(${PROJECT_NAME} "../../Assets/Models" "Models")
copy_assets(${PROJECT_NAME} "../../Assets/Textures" "Textures")
compile_shaders(${PROJECT_NAME} "../../Assets/Shaders")