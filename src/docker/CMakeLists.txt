
project(SphinxDocker CXX)

include_directories(${SphinxSharedLib_SOURCE_DIR})
include_directories(${SphinxDocker_SOURCE_DIR})
file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/*.cpp)
add_library(SphinxDocker ${SRC_FILES})

add_dependencies(SphinxDocker json) 
target_link_libraries(SphinxDocker LINK_PUBLIC SphinxSharedLib ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} )
