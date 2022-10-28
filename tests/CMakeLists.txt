# Tests need to be added as executables first
add_executable(testlib test.cpp)

# I'm using C++17 in the test
target_compile_features(testlib PRIVATE cxx_std_17)

target_link_libraries(testlib PRIVATE msmarco_search_engine)

# If you register a test, then ctest and make test will run it.
# You can also run examples and check the output, as well.
add_test(NAME testlibtest COMMAND testlib) # Command can be a target
