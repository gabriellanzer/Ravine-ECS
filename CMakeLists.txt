cmake_minimum_required(VERSION 3.1.0)

# Project Setup
project(Ravine-ECS VERSION 0.1.0)

# Usefull Definitions
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(CMAKE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

# Configure Output Paths
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/)
# Configure Libs
include_directories(${CMAKE_INCLUDE_DIR})

# Configure Executable
include_directories(${CMAKE_SRC_DIR})
file(GLOB_RECURSE SRC_FILES "${CMAKE_SRC_DIR}/*.cpp")
add_executable(ravine-ecs ${SRC_FILES})

# Target for C++ STD Version 2017
target_compile_features(ravine-ecs PRIVATE cxx_std_17)
target_compile_options(ravine-ecs PRIVATE -I -pthread)
target_link_options(ravine-ecs PRIVATE)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
