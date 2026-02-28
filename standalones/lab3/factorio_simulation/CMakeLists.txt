cmake_minimum_required(VERSION 3.16)
project(FactorioSimulation VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()


set(SOURCES
        src/main.cpp
        src/assembler.cpp
        src/quality.cpp
        src/recipe.cpp
        src/inventory.cpp
        src/recycler.cpp
        src/simulation.cpp
        src/simulation.hpp
        src/reporter.cpp
        src/config_parser.cpp
)

add_executable(factorio_simulation ${SOURCES})

target_include_directories(factorio_simulation PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)


find_program(PDFLATEX_EXECUTABLE xelatex)
if(PDFLATEX_EXECUTABLE)
    message(STATUS "pdflatex found: ${PDFLATEX_EXECUTABLE}")
    target_compile_definitions(factorio_simulation PRIVATE PDFLATEX_AVAILABLE)
else()
    message(WARNING "pdflatex not found — Reporter will produce .tex only, no auto-compile to PDF")
endif()




install(TARGETS factorio_simulation DESTINATION bin)
