FetchContent_Declare(
  catch
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v2.13.6)
FetchContent_MakeAvailable(catch)

add_executable(posting_lists_test posting_lists_test.cpp)
target_link_libraries(posting_lists_test PRIVATE msmarco_search_engine Catch2::Catch2)
add_test(NAME testlibtest COMMAND posting_lists_test)

add_executable(hash_map_test hash_map_test.cpp)
target_link_libraries(hash_map_test PRIVATE msmarco_search_engine Catch2::Catch2)
