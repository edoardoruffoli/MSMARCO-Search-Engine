#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "MSMARCO-Search-Engine/parsing.hpp"

TEST_CASE("TokenizerTest", "Spaces") {
    std::unordered_map<std::string, int> tokens;
    std::unordered_set<std::string> stopwords;
    std::string content;

    content = "ciao ciao ciao";
    tokenize(content, false, stopwords, tokens);
    REQUIRE(tokens["ciao"] == 3);

    tokens.clear();
    content = " ciao  ciao  ciao ";
    tokenize(content, false, stopwords, tokens);
    REQUIRE(tokens["ciao"] == 3);

    tokens.clear();
    content = "\tciao\tciao\tciao";
    tokenize(content, false, stopwords, tokens);
    REQUIRE(tokens["ciao"] == 3);

    tokens.clear();
    content = "\t\tciao \tciao\t ciao";
    tokenize(content, false, stopwords, tokens);
    REQUIRE(tokens["ciao"] == 3);
}
