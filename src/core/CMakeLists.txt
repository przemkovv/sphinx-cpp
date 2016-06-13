
project(SphinxCore CXX)

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )

set( CMAKE_EXPORT_COMPILE_COMMANDS 1 )

include_directories(${SphinxCore_SOURCE_DIR})
file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/*.cpp)
add_library(SphinxCore ${SRC_FILES})

target_link_libraries(SphinxCore PUBLIC ${POCO_UTIL} ${POCO_FOUNDATION} ${POCO_NET})
