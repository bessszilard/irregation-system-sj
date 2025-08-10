cmake_minimum_required(VERSION 3.10)

# Project name
project(MyProject)

# Define a macro for PC builds
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_definitions(-DPC_BUILD)
endif()

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the main.cpp file
# add_executable(MyProject main.cpp)

# Enable testing
enable_testing()

# Add the test directory
add_subdirectory(test)
add_subdirectory(simulation)