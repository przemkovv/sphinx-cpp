
project(SphinxDocker CXX)

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )

set( CMAKE_EXPORT_COMPILE_COMMANDS 1 )
include_directories(${SphinxCore_SOURCE_DIR})
include_directories(${SphinxDocker_SOURCE_DIR})
file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/*.cpp)
add_executable(SphinxDocker ${SRC_FILES})

target_link_libraries(SphinxDocker LINK_PUBLIC SphinxCore ${CRYPTO_LIB} ${SSL_LIB} ${NGHTTP2_LIB} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} )
