
project(SphinxApp CXX)

set( CMAKE_EXPORT_COMPILE_COMMANDS 1 )
include_directories(${SphinxCore_SOURCE_DIR})
include_directories(${SphinxApp_SOURCE_DIR})
file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/*.cpp)
add_executable(SphinxApp ${SRC_FILES})

target_link_libraries(SphinxApp LINK_PUBLIC SphinxCore ${CMAKE_THREAD_LIBS_INIT} )
