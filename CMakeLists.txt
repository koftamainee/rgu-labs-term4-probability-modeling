cmake_minimum_required(VERSION 3.25)
project(rgu-labs-term4-probability-modeling LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
include_directories(include)

file(GLOB TASK_SOURCES "src/*.cpp")
foreach(src_file ${TASK_SOURCES})
    get_filename_component(task_name ${src_file} NAME_WE)
    add_executable(${task_name} ${src_file})
endforeach()
