
add_definitions(
    -I${CMAKE_CURRENT_SOURCE_DIR}
    )

file(GLOB_RECURSE Sphinx_SOURCES "*.cpp")
file(GLOB_RECURSE Sphinx_HEADERS "*.h")

set (Sphinx_INCLUDE_DIRS "")
foreach (_headerFile ${Sphinx_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND Sphinx_INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES Sphinx_INCLUDE_DIRS)

include_directories(${Sphinx_INCLUDE_DIRS})
add_executable(sphinx ${Sphinx_SOURCES})

find_library(POCO_UTIL PocoUtil)
find_library(POCO_FOUNDATION PocoFoundation)
find_library(POCO_NET PocoNet)
find_library(LIBCLANG_LIBRARY NAMES clang)

target_link_libraries(sphinx ${POCO_FOUNDATION} ${POCO_UTIL} ${POCO_NET} ${LIBCLANG_LIBRARY})
