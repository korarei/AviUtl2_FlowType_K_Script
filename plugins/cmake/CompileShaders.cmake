function(compile_pixel_shaders TARGET_NAME SHADER_SOURCES)
    # Find fxc.exe for shader compilation.
    find_program(FXC_EXECUTABLE fxc)
    if(NOT FXC_EXECUTABLE)
        message(FATAL_ERROR "fxc.exe not found.")
    endif()

    set(SHADER_DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/shaders/")
    file(MAKE_DIRECTORY "${SHADER_DESTINATION}")

    set(SHADER_HEADERS "")

    # Build shaders.
    foreach(SHADER IN LISTS SHADER_SOURCES)
        get_filename_component(FILE_NAME "${SHADER}" NAME)

        if(FILE_NAME MATCHES "^(.+)\\.hlsl$")
            set(SHADER_NAME "${CMAKE_MATCH_1}")
            set(SHADER_HEADER "${SHADER_DESTINATION}/${SHADER_NAME}.h")

            add_custom_command(
                OUTPUT  "${SHADER_HEADER}"
                COMMAND "${FXC_EXECUTABLE}"
                /T "ps_5_0"
                /E main
                /O3
                /WX
                /Qstrip_reflect
                /Qstrip_debug
                /Fh "${SHADER_HEADER}"
                /Vn "g_${SHADER_NAME}"
                "${SHADER}"
                DEPENDS "${SHADER}"
                VERBATIM
            )

            list(APPEND SHADER_HEADERS "${SHADER_HEADER}")
        endif()
    endforeach()

    add_custom_target(${TARGET_NAME}_PixelShaders DEPENDS ${SHADER_HEADERS})
    add_dependencies(${TARGET_NAME} ${TARGET_NAME}_PixelShaders)
    target_include_directories(${TARGET_NAME} PUBLIC "${SHADER_DESTINATION}")
endfunction()

function(compile_compute_shaders TARGET_NAME SHADER_SOURCES)
    # Find fxc.exe for shader compilation.
    find_program(FXC_EXECUTABLE fxc)
    if(NOT FXC_EXECUTABLE)
        message(FATAL_ERROR "fxc.exe not found.")
    endif()

    set(SHADER_DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/shaders/")
    file(MAKE_DIRECTORY "${SHADER_DESTINATION}")

    set(SHADER_HEADERS "")

    # Build shaders.
    foreach(SHADER IN LISTS SHADER_SOURCES)
        get_filename_component(FILE_NAME "${SHADER}" NAME)

        if(FILE_NAME MATCHES "^(.+)\\.hlsl$")
            set(SHADER_NAME "${CMAKE_MATCH_1}")
            set(SHADER_HEADER "${SHADER_DESTINATION}/${SHADER_NAME}.h")

            add_custom_command(
                OUTPUT  "${SHADER_HEADER}"
                COMMAND "${FXC_EXECUTABLE}"
                /T "cs_5_0"
                /E main
                /O3
                /WX
                /Qstrip_reflect
                /Qstrip_debug
                /Fh "${SHADER_HEADER}"
                /Vn "g_${SHADER_NAME}"
                "${SHADER}"
                DEPENDS "${SHADER}"
                VERBATIM
            )

            list(APPEND SHADER_HEADERS "${SHADER_HEADER}")
        endif()
    endforeach()

    add_custom_target(${TARGET_NAME}_ComputeShaders DEPENDS ${SHADER_HEADERS})
    add_dependencies(${TARGET_NAME} ${TARGET_NAME}_ComputeShaders)
    target_include_directories(${TARGET_NAME} PUBLIC "${SHADER_DESTINATION}")
endfunction()
