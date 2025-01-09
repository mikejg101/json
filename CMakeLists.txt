cmake_minimum_required(VERSION 3.30)
project(json VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)

# Find Boost
find_package(Boost REQUIRED COMPONENTS program_options filesystem json)

# Add executable
add_executable(json main.cpp)

# Link Boost libraries
target_link_libraries(json Boost::program_options Boost::filesystem Boost::json)

# Generate version header file
configure_file(
        "${PROJECT_SOURCE_DIR}/version.h.in"
        "${PROJECT_BINARY_DIR}/version.h"
)
include_directories("${PROJECT_BINARY_DIR}")

install(TARGETS json DESTINATION bin)