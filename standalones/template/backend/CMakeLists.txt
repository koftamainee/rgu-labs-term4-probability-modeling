cmake_minimum_required(VERSION 3.16)
project(backend CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Dependencies ──────────────────────────────────────────────
include(FetchContent)

FetchContent_Declare(
    crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG        v1.2.0
)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)

FetchContent_MakeAvailable(crow nlohmann_json)

# ── Sources ───────────────────────────────────────────────────
file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(backend ${SOURCES})

target_link_libraries(backend PRIVATE
    Crow::Crow
    nlohmann_json::nlohmann_json
)

# Crow requires pthread and asio
find_package(Threads REQUIRED)
target_link_libraries(backend PRIVATE Threads::Threads)
