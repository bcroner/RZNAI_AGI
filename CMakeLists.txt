cmake_minimum_required(VERSION 3.16)
project(RZNAI_AGI_Core VERSION 1.0.0 LANGUAGES CXX)

# Force strict modern C++ compliance profiles
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Optimize for high-performance computing targets
if(MSVC)
    add_compile_options(/O2 /W4 /permissive-)
else()
    add_compile_options(-O3 -Wall -Wextra -march=native -pthread)
endif()

# Find and configure the required native multi-threading architecture
find_package(Threads REQUIRED)

# Define core engine library nodes
add_library(rzn_agi_core STATIC
    src/AgiBroker.cpp
    src/TaskScheduler.cpp
)
target_include_directories(rzn_agi_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(rzn_agi_core PRIVATE Threads::Threads)

# Define primary deployment orchestration node
add_executable(rzn_agi_node src/main.cpp)
target_link_libraries(rzn_agi_node PRIVATE rzn_agi_core)
