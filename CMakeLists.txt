cmake_minimum_required(VERSION 3.10)

# Project name
project(JAM)

# Use C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# First executable
add_executable(JAM
    src/JAM.cpp
)

# Second executable
add_executable(JLD
    src/JLD.cpp
)

# Include directory for both executables
target_include_directories(JAM PRIVATE
    include
)

target_include_directories(JLD PRIVATE
    include
)