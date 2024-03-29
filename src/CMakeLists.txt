﻿file(GLOB HEADER_LIST "${MSMARCO-Search-Engine_SOURCE_DIR}/include/MSMARCO-Search-Engine/*.hpp")

add_library(msmarco_search_engine 
                parsing.cpp 
                posting_list.cpp
                merging.cpp 
                query.cpp 
                compressing.cpp 
                scoring_functions.cpp
                lexicon.cpp
                doc_table.cpp
                 ${HEADER_LIST})

target_include_directories(msmarco_search_engine PUBLIC ../include ../thirdparty)

if(UNIX)
    find_package(ZLIB)
    find_package(Boost REQUIRED system chrono serialization filesystem iostreams)
endif()

include_directories(${ZLIB_INCLUDE_DIRS})

target_link_libraries(msmarco_search_engine PUBLIC Boost::boost Boost::chrono Boost::serialization Boost::filesystem Boost::iostreams ${ZLIB_LIBRARIES} Porter2)
target_compile_features(msmarco_search_engine PUBLIC cxx_std_17)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/tmp)

source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})
