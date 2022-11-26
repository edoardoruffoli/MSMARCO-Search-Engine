execute_process(COMMAND git submodule update --init --recursive)

add_subdirectory(boost-cmake)
add_subdirectory(zlib)
add_subdirectory(Porter2)
add_subdirectory(cli)

# stxxl
add_definitions(-DSTXXL_VERBOSE_LEVEL=-10) # suppress messages to stdout
add_subdirectory(stxxl)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${STXXL_CXX_FLAGS}")