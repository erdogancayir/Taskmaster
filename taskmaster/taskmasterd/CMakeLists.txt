# Proje tanımı
project(taskmasterd)

# yaml-cpp yolunu ayarla
set(YAML_CPP_INCLUDE_DIR "/opt/homebrew/Cellar/yaml-cpp/0.8.0/include")
set(YAML_CPP_LIBRARIES "/opt/homebrew/Cellar/yaml-cpp/0.8.0/lib/libyaml-cpp.dylib")

# Header ve kaynak dosyaları
include_directories(
    include
    include/deamon
    ${YAML_CPP_INCLUDE_DIR}
)

file(GLOB SOURCES
    src/*.cpp
    src/deamon/*.cpp
)

add_executable(taskmasterd ${SOURCES})
target_link_libraries(taskmasterd ${YAML_CPP_LIBRARIES})