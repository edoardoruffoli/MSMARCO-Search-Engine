file(GLOB HEADER_LIST "${MSMARCO-Search-Engine_SOURCE_DIR}/include/MSMARCO-Search-Engine/*.hpp")

add_library(msmarco_search_engine 
                parsing.cpp 
                posting_list.cpp
                merging.cpp 
                io.cpp 
                query.cpp 
                compressing.cpp 
                simple9.cpp ${HEADER_LIST})

target_include_directories(msmarco_search_engine PUBLIC ../include)

#find_package(ZLIB)

#OpenMP
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(msmarco_search_engine PUBLIC OpenMP::OpenMP_CXX ${llvm_libs})
endif()

include_directories(${ZLIB_INCLUDE_DIRS})
target_link_libraries(msmarco_search_engine PUBLIC Boost::boost Boost::serialization Boost::filesystem Boost::iostreams ${ZLIB_LIBRARIES} Porter2)
target_compile_features(msmarco_search_engine PUBLIC cxx_std_17)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/tmp)

source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})
