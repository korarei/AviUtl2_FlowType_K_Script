cmake_minimum_required(VERSION 3.20.0)

project(DeformFilter LANGUAGES CXX)

add_library(${PROJECT_NAME} STATIC
    intern/deform.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        AviUtl2SDK
    PRIVATE
        Vector
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
