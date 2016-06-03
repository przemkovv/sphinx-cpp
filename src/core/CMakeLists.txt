
project(SphinxCore CXX)

set( CMAKE_EXPORT_COMPILE_COMMANDS 1 )

include_directories(${SphinxCore_SOURCE_DIR})
file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/*.cpp)
add_library(SphinxCore ${SRC_FILES})

target_link_libraries(SphinxCore PUBLIC ${Boost_LIBRARIES} ${POCO_UTIL} ${POCO_FOUNDATION} ${POCO_NET})
