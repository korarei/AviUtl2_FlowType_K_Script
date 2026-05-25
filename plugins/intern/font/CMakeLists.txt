cmake_minimum_required(VERSION 3.20.0)

project(Font LANGUAGES CXX)

find_package(harfbuzz CONFIG REQUIRED)

add_library(${PROJECT_NAME} STATIC
    intern/font.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        harfbuzz::harfbuzz
        dwrite
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
