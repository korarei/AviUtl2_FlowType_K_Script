cmake_minimum_required(VERSION 3.20.0)

project(AlignFilter LANGUAGES CXX)

add_library(${PROJECT_NAME} STATIC
    intern/align.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        AviUtl2SDK
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
