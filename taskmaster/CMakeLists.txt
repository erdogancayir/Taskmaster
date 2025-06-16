cmake_minimum_required(VERSION 3.10)
project(Taskmaster)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(taskmasterd)
add_subdirectory(taskmasterctl)