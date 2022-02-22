cmake_minimum_required(VERSION 3.1.0)

# Project Setup
project(Ravine-ECS VERSION 0.1.1)

# Configure Conan
include(${CMAKE_SOURCE_DIR}/conan.cmake)

conan_cmake_run(REQUIRES
				fmt/7.1.3
				taskflow/3.0.0
                BASIC_SETUP NO_OUTPUT_DIRS
				IMPORTS "., license* -> ../../licenses @ folder=True, ignore_case=True"
				IMPORTS "include, * -> ../../include @ ignore_case=True"
				CMAKE_TARGETS BUILD missing)

# Configure Includes
include_directories(${CMAKE_SOURCE_DIR}/include)

# Configure Executable
include_directories(${CMAKE_SOURCE_DIR}/src)
file(GLOB_RECURSE SRC_FILES "${CMAKE_SOURCE_DIR}/src/*.c*")
add_executable(ravine-ecs ${SRC_FILES})

# Target for C++ STD Version 2017
target_compile_features(ravine-ecs PRIVATE cxx_std_17)
target_compile_options(ravine-ecs PRIVATE -I -pthread)
target_link_options(ravine-ecs PRIVATE -mwindows)

target_link_libraries(ravine-ecs CONAN_PKG::fmt)

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
