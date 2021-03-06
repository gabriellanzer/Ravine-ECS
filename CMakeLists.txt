cmake_minimum_required(VERSION 3.1.0)

# Project Setup
project(Ravine-ECS VERSION 0.1.1)

# Configure Conan
include(${CMAKE_SOURCE_DIR}/build/conanbuildinfo_multi.cmake)
conan_basic_setup() # Do not change CMAKE output dirs

# Configure Includes
include_directories(${CMAKE_SOURCE_DIR}/include)

# Configure Executable
include_directories(${CMAKE_SOURCE_DIR}/src)
file(GLOB_RECURSE SRC_FILES "${CMAKE_SOURCE_DIR}/src/*.c*")
add_executable(ravine-ecs ${SRC_FILES})

# Target for C++ STD Version 2017
#add_definitions("-std=c++17")
target_compile_features(ravine-ecs PRIVATE cxx_std_17)
target_compile_options(ravine-ecs PRIVATE -I -pthread)
conan_target_link_libraries(ravine-ecs)
target_link_options(ravine-ecs PRIVATE -mwindows)

set_target_properties(
    ravine-ecs
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/${CMAKE_BUILD_TYPE}/"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/${CMAKE_BUILD_TYPE}/"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/"
    OUTPUT_NAME "Ravine-ECS ${PROJECT_VERSION}"
)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
