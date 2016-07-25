
project(SphinxApp CXX)

include_directories(${SphinxSharedLib_SOURCE_DIR})
include_directories(${SphinxCore_SOURCE_DIR})
include_directories(${SphinxApp_SOURCE_DIR})
include_directories(${Sphinx_SOURCE_DIR}/src/docker)
file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/*.cpp)
add_executable(SphinxApp ${SRC_FILES})

target_link_libraries(SphinxApp LINK_PUBLIC SphinxSharedLib SphinxCore SphinxDocker ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${FMT} ${POCO_FOUNDATION} ${POCO_NET})
