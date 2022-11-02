execute_process(COMMAND git submodule update --init --recursive)

add_subdirectory(boost-cmake)
include_directories(zlib)
