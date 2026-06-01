cmake_minimum_required(VERSION 3.20.0)

project(TrimFilter LANGUAGES CXX)

find_package(Eigen3 CONFIG REQUIRED)

add_library(${PROJECT_NAME} STATIC
    intern/trim.cpp
)

include(${CMAKE_SCRIPTS}/CompileShaders.cmake)
set(SHADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/intern/shaders/blit.hlsl"
)
compile_pixel_shaders(${PROJECT_NAME} "${SHADERS}")

target_include_directories(${PROJECT_NAME}
    PUBLIC
        "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        AviUtl2SDK
    PRIVATE
        Eigen3::Eigen
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
