cmake_minimum_required(VERSION 3.10)
project(Taskmaster)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# yaml-cpp paths
set(YAML_CPP_INCLUDE_DIR "/opt/homebrew/Cellar/yaml-cpp/0.8.0/include")
set(YAML_CPP_LIBRARIES "/opt/homebrew/Cellar/yaml-cpp/0.8.0/lib/libyaml-cpp.dylib")

include_directories(include ${YAML_CPP_INCLUDE_DIR})
link_directories(/opt/homebrew/Cellar/yaml-cpp/0.8.0/lib)

file(GLOB SOURCES "src/*.cpp")
add_executable(taskmaster ${SOURCES})

target_link_libraries(taskmaster ${YAML_CPP_LIBRARIES})