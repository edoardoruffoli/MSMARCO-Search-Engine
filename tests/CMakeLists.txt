Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v2.13.6)

FetchContent_MakeAvailable(Catch2)

add_executable(compressing_test compressing_test.cpp)
target_link_libraries(compressing_test PRIVATE msmarco_search_engine Catch2::Catch2)

add_executable(doc_table_test doc_table_test.cpp)
target_link_libraries(doc_table_test PRIVATE msmarco_search_engine Catch2::Catch2)

add_executable(lexicon_test lexicon_test.cpp)
target_link_libraries(lexicon_test PRIVATE msmarco_search_engine Catch2::Catch2)

add_executable(posting_lists_test posting_lists_test.cpp)
target_link_libraries(posting_lists_test PRIVATE msmarco_search_engine Catch2::Catch2)

add_executable(tokenizer_test tokenizer_test.cpp)
target_link_libraries(tokenizer_test PRIVATE msmarco_search_engine Catch2::Catch2)
