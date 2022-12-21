execute_process(COMMAND git submodule update --init --recursive)

if(WIN32)
	add_subdirectory(boost-cmake)
	add_subdirectory(zlib)
endif()
add_subdirectory(Porter2)
add_subdirectory(cli)
