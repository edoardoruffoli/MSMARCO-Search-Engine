#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "MSMARCO-Search-Engine/parsing.hpp"

TEST_CASE("TokenizerTest", "Splitting") {
    std::unordered_map<std::string, int> tokens;
    std::unordered_set<std::string> stopwords;

    std::string content = "ciao ciao ciao";
    tokenize(content, false, stopwords, tokens);

    REQUIRE(tokens["ciao"] == 3);
}
