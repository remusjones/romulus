function(compile_shaders TARGET_NAME RELATIVE_SHADER_PATH)
    get_filename_component(FULL_SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${RELATIVE_SHADER_PATH}" ABSOLUTE)

    set(DEST_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Assets/Shaders")

    file(GLOB_RECURSE SHADERS CONFIGURE_DEPENDS
            "${FULL_SHADER_DIR}/*.vert"
            "${FULL_SHADER_DIR}/*.frag"
            "${FULL_SHADER_DIR}/*.glsl"
    )
    file(GLOB_RECURSE SHADER_INCLUDES CONFIGURE_DEPENDS "${FULL_SHADER_DIR}/*.inc")

    foreach(SHADER ${SHADERS})
        get_filename_component(SHADER_NAME ${SHADER} NAME)  # Keep extension (e.g. main.vert)
        set(OUTPUT_FILE "${DEST_DIR}/${SHADER_NAME}.spv")   # finally main.vert.spv

        add_custom_command(
                OUTPUT "${OUTPUT_FILE}"
                COMMAND ${CMAKE_COMMAND} -E make_directory "${DEST_DIR}"
                COMMAND ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} "${SHADER}" -o "${OUTPUT_FILE}" -V
                MAIN_DEPENDENCY "${SHADER}"
                DEPENDS ${SHADER_INCLUDES}
                COMMENT "Compiling Shader: ${SHADER_NAME} -> ${OUTPUT_FILE}"
                VERBATIM
        )

        set(SHADER_TARGET "Shader_${SHADER_NAME}")
        if(NOT TARGET ${SHADER_TARGET})
            add_custom_target(${SHADER_TARGET} DEPENDS "${OUTPUT_FILE}")
        endif()

        add_dependencies(${TARGET_NAME} ${SHADER_TARGET})
    endforeach()
endfunction()