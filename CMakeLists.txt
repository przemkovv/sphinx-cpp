cmake_minimum_required(VERSION 3.0)

project(Sphinx)

add_definitions(
    -std=c++1y
    )

find_library(POCO_FOUNDATION PocoFoundation)
find_library(POCO_NET PocoNet)
find_library(POCO_UTIL PocoUtil)

add_subdirectory(src/core)
#add_subdirectory(src/net)
add_subdirectory(src/app)
add_subdirectory(src/tests)



