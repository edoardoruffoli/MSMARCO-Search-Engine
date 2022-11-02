execute_process(COMMAND git submodule update --init --recursive)

add_subdirectory(boost-cmake)

if(WIN32)
    set(ZLIB_LIBRARY "zlib/lib/zlib.lib")
    set(ZLIB_INCLUDE_DIR "zlib/include")
endif()