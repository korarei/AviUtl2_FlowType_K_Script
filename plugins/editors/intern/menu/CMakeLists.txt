cmake_minimum_required(VERSION 3.20.0)

project(Menu LANGUAGES CXX)

add_subdirectory(intern/object)
add_subdirectory(intern/property)

# Library definition.
add_library(${PROJECT_NAME} STATIC
    intern/menu.cpp
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
        Object
        Property
)

# C++ version specification.
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
