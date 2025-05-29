cmake_minimum_required(VERSION 3.10)
project(Taskmaster)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set yaml-cpp paths
set(YAML_CPP_INCLUDE_DIR "/opt/homebrew/Cellar/yaml-cpp/0.8.0/include")
set(YAML_CPP_LIBRARIES "/opt/homebrew/Cellar/yaml-cpp/0.8.0/lib/libyaml-cpp.dylib")

# Header dosyalarını ekle
include_directories(include ${YAML_CPP_INCLUDE_DIR})

# Add yaml-cpp library directory to linker search path
link_directories(/opt/homebrew/Cellar/yaml-cpp/0.8.0/lib)

# Ana derlenebilir hedef
add_executable(taskmaster
    src/main.cpp
    src/ConfigParser.cpp
)

# yaml-cpp linkleme
target_link_libraries(taskmaster ${YAML_CPP_LIBRARIES})