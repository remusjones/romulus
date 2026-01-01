 add_subdirectory(Shaders)

 # Function to copy any asset folder (Models, Textures, etc.)
 # to the project's bin/Assets/ folder.
function(copy_assets TARGET_NAME SOURCE_DIR DEST_SUBFOLDER)
    get_filename_component(ABS_SOURCE_DIR "${SOURCE_DIR}" ABSOLUTE)

    set(DEST_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Assets/${DEST_SUBFOLDER}")

    message(STATUS "Copying Assets: ${ABS_SOURCE_DIR} -> ${DEST_DIR}")

    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${DEST_DIR}"
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${ABS_SOURCE_DIR}" "${DEST_DIR}"
            COMMENT "Copying assets ${DEST_SUBFOLDER} assets for ${TARGET_NAME}..."
            VERBATIM
    )
endfunction()