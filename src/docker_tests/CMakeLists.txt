
project(SphinxDockerTests CXX)


include_directories(${SphinxSharedLib_SOURCE_DIR})
include_directories(${SphinxDocker_SOURCE_DIR})
file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/*.cpp)
add_executable(SphinxDockerTests ${SRC_FILES} )

#target_link_libraries(SphinxDocker LINK_PUBLIC SphinxCore ${CRYPTO_LIB} ${SSL_LIB} ${NGHTTP2_LIB} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} )
target_link_libraries(SphinxDockerTests LINK_PUBLIC SphinxSharedLib SphinxDocker ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${FMT} )
