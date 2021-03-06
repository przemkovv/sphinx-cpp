cmake_minimum_required(VERSION 3.0)

project(Sphinx)


add_definitions(
    -std=c++1z
    -g
    #-O3
    #-Wall
    -Weverything
    -Wno-c++98-compat
    -Wno-c++98-compat-pedantic
    -Wno-exit-time-destructors
    -Wno-global-constructors
    -Wno-padded
    -Wno-weak-vtables
    -Wno-shadow
    -Weffc++
    -Werror
    -DSPDLOG_FMT_EXTERNAL
    )


include_directories(SYSTEM ${PROJECT_BINARY_DIR}/include)


find_library(POCO_FOUNDATION PocoFoundation)
find_library(POCO_NET PocoNet)
find_library(FMT fmt libfmt)
find_package (Threads REQUIRED)
find_package(Boost 1.60 COMPONENTS system filesystem program_options)

include(ExternalProject)

ExternalProject_Add(json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    INSTALL_DIR ${PROJECT_BINARY_DIR}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> -DBuildTests=OFF
    UPDATE_COMMAND ""
    )

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )

set( CMAKE_EXPORT_COMPILE_COMMANDS 1 )
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_COLOR_MAKEFILE   ON)

add_subdirectory(src/shared_lib)
add_subdirectory(src/docker)
add_subdirectory(src/core)
#add_subdirectory(src/net)
add_subdirectory(src/app)
add_subdirectory(src/docker_tests)
add_subdirectory(src/tests)



