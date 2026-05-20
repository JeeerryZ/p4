cmake_minimum_required(VERSION 3.16)
project(wp4-backend CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Boost is required by Crow (header-only for Crow's async I/O)
find_package(Boost REQUIRED COMPONENTS system)

# Include paths
include_directories(
    ${CMAKE_SOURCE_DIR}/third_party/crow
    ${CMAKE_SOURCE_DIR}/src
    ${Boost_INCLUDE_DIRS}
)

# Source files
set(SOURCES
    src/main.cc
    src/ScriptHandler.cc
    src/file_tab.cc
    src/math_polynom.cc
    src/api/EvaluateHandler.cc
    src/models/EvaluateResponse.cc
)

add_executable(wp4-backend ${SOURCES})

target_link_libraries(wp4-backend
    ${Boost_LIBRARIES}
    pthread
)

# Silence MSVC/clangd "deprecated" warnings for fopen/fscanf/strcpy on Windows.
# These are standard POSIX functions; this define is a no-op on Linux/GCC.
target_compile_definitions(wp4-backend PRIVATE _CRT_SECURE_NO_WARNINGS)

# Compiler flags
target_compile_options(wp4-backend PRIVATE
    -Wall
    -Wextra
    -Wno-unused-result
    -Wno-write-strings
    -Wno-unused-parameter
)

# Optional: installation
install(TARGETS wp4-backend DESTINATION bin)
