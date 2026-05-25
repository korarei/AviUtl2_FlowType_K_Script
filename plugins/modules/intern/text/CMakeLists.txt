cmake_minimum_required(VERSION 3.20.0)

project(Text LANGUAGES CXX)

find_package(Eigen3 CONFIG REQUIRED)
find_package(re2 CONFIG REQUIRED)
find_package(utf8cpp CONFIG REQUIRED)

# Library definition.
add_library(${PROJECT_NAME} STATIC
    intern/kerning.cpp
    intern/regex.cpp
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
        re2::re2
        utf8cpp::utf8cpp
        Font
)

# C++ version specification.
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
