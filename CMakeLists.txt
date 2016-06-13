cmake_minimum_required(VERSION 3.0)

project(Sphinx)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_definitions(
    -std=c++1z
    -g
    #-Wall
    -Weverything
    -Wno-c++98-compat
    -Wno-exit-time-destructors
    -Wno-global-constructors
    -Wno-padded
    -Wno-weak-vtables
    -Wno-shadow
    -Weffc++
    -Werror
    )
#INSTALL(TARGETS HelloCMake DESTINATION ${HelloCMake_SOURCE_DIR}/assets)
find_library(POCO_FOUNDATION PocoFoundation)
find_library(POCO_NET PocoNet)
#find_library(POCO_UTIL PocoUtil)
find_package (Threads REQUIRED)
find_package(Boost 1.60 COMPONENTS filesystem program_options)

add_subdirectory(src/core)
#add_subdirectory(src/net)
add_subdirectory(src/app)
#add_subdirectory(src/tests)



