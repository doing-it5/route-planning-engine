cmake_minimum_required(VERSION 3.14)
project(RoutePlanningEngine
    VERSION 1.0.0
    DESCRIPTION "High-performance C++ OpenStreetMap shortest-path & route planning engine"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Prevent Windows.h from defining min/max macros
add_compile_definitions(NOMINMAX)

# Core Library Sources
set(LIB_SOURCES
    src/graph/Graph.cpp
    src/algorithms/Dijkstra.cpp
    src/algorithms/AStar.cpp
    src/algorithms/HeldKarp.cpp
    src/algorithms/TwoOpt.cpp
    src/benchmark/Benchmark.cpp
    src/parser/OSMParser.cpp
    src/utils/Utils.cpp
)

# Static Library Target
add_library(route_planning_lib STATIC ${LIB_SOURCES})
target_include_directories(route_planning_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(MSVC)
    target_compile_options(route_planning_lib PRIVATE /EHsc /W4)
else()
    target_compile_options(route_planning_lib PRIVATE -Wall -Wextra -Wpedantic)
endif()

# CLI Executable Target
add_executable(route_planner src/main.cpp)
target_link_libraries(route_planner PRIVATE route_planning_lib)

if(MSVC)
    target_compile_options(route_planner PRIVATE /EHsc /W4)
else()
    target_compile_options(route_planner PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Testing Support
option(BUILD_TESTING "Build unit tests" ON)
if(BUILD_TESTING)
    enable_testing()
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    # Prevent overriding runtime library on Windows
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_subdirectory(tests)
endif()
