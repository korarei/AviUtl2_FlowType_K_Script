cmake_minimum_required(VERSION 3.20.0)

project(Module LANGUAGES CXX)

add_subdirectory(intern/hash)
add_subdirectory(intern/vector)
add_subdirectory(intern/trim)
add_subdirectory(intern/island)
add_subdirectory(intern/toml)
add_subdirectory(intern/text)

add_library(${PROJECT_NAME} STATIC
    intern/module.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        AviUtl2SDK
    PRIVATE
        Hash
        Island
        Text
        Toml
        Trim
        Vector
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
