execute_process(COMMAND git submodule update --init --recursive)

add_subdirectory(boost-cmake)
add_subdirectory(zlib)
add_subdirectory(Porter2)
