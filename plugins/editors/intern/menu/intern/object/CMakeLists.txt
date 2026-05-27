cmake_minimum_required(VERSION 3.20.0)

project(ObjectMenu LANGUAGES CXX)

find_package(Eigen3 CONFIG REQUIRED)
find_package(utf8cpp CONFIG REQUIRED)

# Library definition.
add_library(${PROJECT_NAME} STATIC
    intern/object.cpp
    intern/text.cpp
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
        Eigen3::Eigen
        utf8cpp::utf8cpp
        Alias
        Font
)

# C++ version specification.
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
