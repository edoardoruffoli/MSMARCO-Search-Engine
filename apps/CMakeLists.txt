add_executable(spimi spimi.cpp)
target_compile_features(spimi PRIVATE cxx_std_17)

target_link_libraries(spimi PRIVATE msmarco_search_engine)
