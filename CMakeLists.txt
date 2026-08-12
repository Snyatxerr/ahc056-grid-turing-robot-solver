cmake_minimum_required(VERSION 3.16)
project(ahc056_grid_turing_robot_solver LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(ahc056_solver src/main.cpp)

if(MSVC)
    target_compile_options(ahc056_solver PRIVATE /W4 /permissive-)
else()
    target_compile_options(ahc056_solver PRIVATE -Wall -Wextra -Wpedantic)
endif()
