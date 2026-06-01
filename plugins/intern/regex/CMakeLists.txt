cmake_minimum_required(VERSION 3.20.0)

project(Regex LANGUAGES CXX)

find_package(re2 CONFIG REQUIRED)

add_library(${PROJECT_NAME} STATIC
    intern/regex.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        re2::re2
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
