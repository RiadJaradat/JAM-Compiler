cmake_minimum_required(VERSION 3.10)

# Project name
project(JAM)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
    src/JAM.cpp
    src/JLD.cpp
)

# Add include directory
target_include_directories(${PROJECT_NAME} PRIVATE
    include
)