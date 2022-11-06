# Note that headers are optional, and do not affect add_library, but they will not
# show up in IDEs unless they are listed in add_library.

# Optionally glob, but only for CMake 3.12 or later:
# file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${ModernCMakeExample_SOURCE_DIR}/include/modern/*.hpp")
file(GLOB HEADER_LIST "${MSMARCO-Search-Engine_SOURCE_DIR}/include/MSMARCO-Search-Engine/*.hpp")

# Make an automatic library - will be static or dynamic based on user setting
add_library(msmarco_search_engine parsing.cpp merging.cpp io.cpp query.cpp ${HEADER_LIST})

# We need this directory, and users of our library will need it too
target_include_directories(msmarco_search_engine PUBLIC ../include)

#find_package(ZLIB)
include_directories(${ZLIB_INCLUDE_DIRS})
target_link_libraries(msmarco_search_engine PUBLIC Boost::boost Boost::serialization Boost::filesystem Boost::iostreams ${ZLIB_LIBRARIES})

# All users of this library will need at least C++11
target_compile_features(msmarco_search_engine PUBLIC cxx_std_17)

# Create directory for intermediate results
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/tmp)

# IDEs should put the headers in a nice place
source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})
