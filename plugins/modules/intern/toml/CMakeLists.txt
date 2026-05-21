cmake_minimum_required(VERSION 3.20.0)

project(Toml LANGUAGES CXX)

find_package(tomlplusplus CONFIG REQUIRED)

# Library definition.
add_library(${PROJECT_NAME} STATIC
    intern/toml.cpp
)

# Include directories.
target_include_directories(${PROJECT_NAME}
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Link libraries.
target_link_libraries(${PROJECT_NAME}
    PUBLIC
        AviUtl2SDK
    PRIVATE
        tomlplusplus::tomlplusplus
        Utilities
)

# C++ version specification.
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
