project(taskmasterctl)

include_directories(include)

file(GLOB SOURCES
    src/*.cpp
)

add_executable(taskmasterctl ${SOURCES})