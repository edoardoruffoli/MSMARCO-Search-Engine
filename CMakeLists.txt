cmake_minimum_required (VERSION 3.8)

# Project name and a few useful settings. Other commands can pick up the results
project(
    MSMARCO-Search-Engine
    VERSION 1.0
    DESCRIPTION "Search Engine"
    LANGUAGES CXX)

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

    # Optionally set things like CMAKE_CXX_STANDARD, CMAKE_POSITION_INDEPENDENT_CODE here

    # Let's ensure -std=c++xx instead of -std=g++xx
    set(CMAKE_CXX_EXTENSIONS OFF)

    # Let's nicely support folders in IDEs
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    # Testing only available if this is the main app
    # Note this needs to be done in the main CMakeLists
    # since it calls enable_testing, which must be in the
    # main CMakeLists.
    include(CTest)

endif()


if(WIN32)
    set(ZLIB_LIBRARY ${CMAKE_BINARY_DIR}/thirdparty/zlib/zlib.lib)
    set(ZLIB_INCLUDE_DIR ${CMAKE_BINARY_DIR}/thirdparty/zlib/)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(thirdparty)

add_subdirectory(src)

add_subdirectory(apps)

option(PACKAGE_TESTS "Build the tests" ON)
if(PACKAGE_TESTS)
  #add_subdirectory(tests)
endif()
