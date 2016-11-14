
project(SphinxTests CXX)

include_directories(${SphinxSharedLib_SOURCE_DIR})
include_directories(${SphinxCore_SOURCE_DIR})
include_directories(${SphinxTests_SOURCE_DIR})
file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/*.cpp)
add_executable(SphinxTests ${SRC_FILES})

target_link_libraries(SphinxTests PUBLIC SphinxCore SphinxSharedLib ${CMAKE_THREAD_LIBS_INIT} ${FMT} )
