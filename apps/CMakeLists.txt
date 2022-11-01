add_executable(parse parse.cpp)
add_executable(merge merge.cpp)

target_compile_features(parse PRIVATE cxx_std_17)
target_compile_features(merge PRIVATE cxx_std_17)

target_link_libraries(parse PRIVATE msmarco_search_engine)
target_link_libraries(merge PRIVATE msmarco_search_engine)
