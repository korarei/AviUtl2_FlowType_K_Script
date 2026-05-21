cmake_minimum_required(VERSION 3.25.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Properties.
set(VERSION "0.1.0" CACHE STRING "Project version")

project(FlowType_K VERSION ${VERSION} LANGUAGES CXX)

if (NOT WIN32)
    message(FATAL_ERROR "This project is only for Windows")
endif()

if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "This project is only for x64")
endif()

if (NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^([xX]64|[xX]86_64|[aA][mM][dD]64)$")
    message(FATAL_ERROR "This project is only for x64")
endif()

# Interface Library
add_library(AviUtl2SDK INTERFACE)
target_include_directories(AviUtl2SDK INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/aviutl2_sdk/include/aviutl2_sdk"
)

add_library(Utilities INTERFACE)
target_include_directories(Utilities INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/intern"
)

# Compiler Dependent Options.
if (MSVC)
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "19.50")
        message(FATAL_ERROR "MSVC version 19.50 or higher is required")
    endif()

    # Runtime Linkage.
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

    add_compile_options(
        /utf-8
        /arch:AVX2
        /W4 # Warning Level 4.
        /permissive- # Standards-based mode.

        # Release-only optimizations.
        $<$<CONFIG:Release>:/O2>
        $<$<CONFIG:Release>:/GL> # Link Optimization.
        $<$<CONFIG:Release>:/Gy> # Split by function. (Remove unused functions.)

        # Debug settings.
        $<$<CONFIG:Debug>:/Od>
        $<$<CONFIG:Debug>:/Zi> # Debugging Information.
    )

    add_link_options(
        $<$<CONFIG:Release>:/LTCG> # Runtime Optimization.
        $<$<CONFIG:Debug>:/DEBUG> # Link with debug information.
    )
else()
    message(FATAL_ERROR "This project is only for MSVC")
endif()

add_compile_definitions(
    UNICODE
    _UNICODE
    NOMINMAX
    WIN32_LEAN_AND_MEAN
    EIGEN_MPL2_ONLY
    VERSION=L"${PROJECT_VERSION}"
)

# Subdirectories.
add_subdirectory(intern/font)
add_subdirectory(modules)
add_subdirectory(editors)

# Main target definition.
add_library(${PROJECT_NAME} SHARED
    main.def
    main.cpp
)

# Library Link.
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        AviUtl2SDK
        Editor
        Font
        Module
)

# Output settings.
set_target_properties(${PROJECT_NAME} PROPERTIES
    PREFIX ""
    SUFFIX ".aux2"
)

# C++ version specification.
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
