cmake_minimum_required(VERSION 3.20.0)

project(Filter LANGUAGES CXX)

add_subdirectory(intern/deform)
add_subdirectory(intern/transform)

add_library(${PROJECT_NAME} STATIC
    intern/filter.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        AviUtl2SDK
    PRIVATE
        DeformFilter
        TransformFilter
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
