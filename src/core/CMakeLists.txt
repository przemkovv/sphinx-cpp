
project(SphinxCore CXX)


include_directories(${SphinxSharedLib_SOURCE_DIR})
include_directories(${SphinxCore_SOURCE_DIR})
include_directories(${SphinxDocker_SOURCE_DIR})
file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/*.cpp)
add_library(SphinxCore ${SRC_FILES})

target_link_libraries(SphinxCore PUBLIC SphinxSharedLib SphinxDocker ${Boost_LIBRARIES} )
