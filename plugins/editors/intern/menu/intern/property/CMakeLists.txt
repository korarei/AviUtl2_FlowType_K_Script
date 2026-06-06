cmake_minimum_required(VERSION 3.20.0)

project(PropertyMenu LANGUAGES CXX)

# Library definition.
add_library(${PROJECT_NAME} STATIC
    intern/effect.cpp
    intern/expression.cpp
    intern/property.cpp
    intern/value.cpp
)

# Include directories.
target_include_directories(${PROJECT_NAME}
    PUBLIC
        "${CMAKE_SOURCE_DIR}"
)

# Link libraries.
target_link_libraries(${PROJECT_NAME}
    PUBLIC
        AviUtl2SDK
    PRIVATE
        Alias
)

# C++ version specification.
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
