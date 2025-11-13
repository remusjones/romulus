cmake_minimum_required(VERSION 4.0)

# Find Vulkan
find_package(Vulkan REQUIRED)

# https://github.com/gabime/spdlog.git
message("Adding spdlog")
add_subdirectory(spdlog)
# ------------------------------------

# https://github.com/g-truc/glm.git
add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)
message("Adding glm")
add_subdirectory(glm glm)
# ------------------------------------

# https://github.com/libsdl-org/SDL.git
message("Adding sdl")
set(SDL_STATIC TRUE)
add_subdirectory(SDL)
# ------------------------------------

# https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
message("Adding VulkanMemoryAllocator")
add_subdirectory(VulkanMemoryAllocator VulkanMemoryAllocator)
# ------------------------------------

# https://github.com/tinyobjloader/tinyobjloader.git
message("Adding tinyobjloader")
add_subdirectory(tinyobjloader tinyobjloader)
#target_include_directories(tinyobjloader INTERFACE tinyobjloader)

# ------------------------------------

# https://github.com/ocornut/imgui.git
message("Adding ImGui")
set(IMGUI_PATH imgui)
add_library(imgui STATIC)
set(IMGUI_SOURCES
        "${IMGUI_PATH}/imgui.h"
        "${IMGUI_PATH}/imgui.cpp"
        "${IMGUI_PATH}/imgui_demo.cpp"
        "${IMGUI_PATH}/imgui_draw.cpp"
        "${IMGUI_PATH}/imgui_widgets.cpp"
        "${IMGUI_PATH}/imgui_tables.cpp"
        "${IMGUI_PATH}/backends/imgui_impl_vulkan.cpp"
        "${IMGUI_PATH}/backends/imgui_impl_sdl3.cpp"
)
target_include_directories(imgui PUBLIC ${IMGUI_PATH} ${IMGUI_PATH}/backends)
target_sources(imgui PUBLIC ${IMGUI_SOURCES})
target_link_libraries(imgui SDL3::SDL3-static)
# ------------------------------------

# https://github.com/CedricGuillemet/ImGuizmo.git
message("Adding ImGuizmo")
set(IMGUIZMO_PATH imguizmo)
add_library(imguizmo STATIC)
target_sources(imguizmo PRIVATE
        "${IMGUIZMO_PATH}/GraphEditor.h"
        "${IMGUIZMO_PATH}/ImCurveEdit.h"
        "${IMGUIZMO_PATH}/ImGradient.h"
        "${IMGUIZMO_PATH}/ImGuizmo.h"
        "${IMGUIZMO_PATH}/ImSequencer.h"
        "${IMGUIZMO_PATH}/ImZoomSlider.h"
        "${IMGUIZMO_PATH}/GraphEditor.cpp"
        "${IMGUIZMO_PATH}/ImCurveEdit.cpp"
        "${IMGUIZMO_PATH}/ImGradient.cpp"
        "${IMGUIZMO_PATH}/ImGuizmo.cpp"
        "${IMGUIZMO_PATH}/ImSequencer.cpp"
)
target_include_directories(imguizmo PUBLIC ${IMGUIZMO_PATH})
target_link_libraries(imguizmo PRIVATE imgui)
# ------------------------------------

# https://github.com/nothings/stb.git
message("Adding stb")
target_include_directories(${PROJECT_NAME} PRIVATE "stb/")
# ------------------------------------


# https://github.com/bulletphysics/bullet3.git
message("Adding bullet3")
set(BUILD_UNIT_TESTS OFF)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/bullet3 EXCLUDE_FROM_ALL)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/bullet3/src")
list(APPEND BULLET_LIBRARIES BulletSoftBody BulletDynamics BulletCollision Bullet3Common LinearMath)

cmake_policy(SET CMP0079 NEW)

# ------------------------------------
# Link libraries
target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        Vulkan::Vulkan
        glm::glm
        spdlog::spdlog
        SDL3::SDL3-static
        GPUOpen::VulkanMemoryAllocator
        tinyobjloader
        imguizmo
        imgui
        ${BULLET_LIBRARIES})

