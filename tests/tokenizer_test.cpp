#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "MSMARCO-Search-Engine/parsing.hpp"

TEST_CASE("TokenizerTest1", "Spaces") {
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

TEST_CASE("TokenizerTest2", "Punctuation") {
    std::string str("Ã¢Â€ÂœNo");
    bool exceptionThrown = false;

    try {
        str.erase(std::remove_if(str.begin(), str.end(), [](unsigned char c) {
            return std::ispunct(c);
            }), str.end());
    }
    catch(...) // special exception type
    {
        exceptionThrown = true;
    }
    //std::string::iterator
    REQUIRE(exceptionThrown == false);
}