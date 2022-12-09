#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "MSMARCO-Search-Engine/lexicon.hpp"
#include "MSMARCO-Search-Engine/model.hpp"

TEST_CASE("Lexicon1", "create") {
    Lexicon lexicon;
    
    REQUIRE(lexicon.create("../../output/lexicon_test.bin", 10) == true);

    lexicon_entry le1 = {1, 1, 1, 1};
    REQUIRE(lexicon.insert("prova1", le1) == true);
    lexicon_entry le2 = {2, 2, 2, 2};
    REQUIRE(lexicon.insert("prova2", le2) == true);
    lexicon_entry le3 = {3, 3, 3, 3};
    REQUIRE(lexicon.insert("prova3", le3) == true);

    lexicon.close();
}

TEST_CASE("Lexicon2", "search") {
    Lexicon lexicon;
    REQUIRE(lexicon.open("../../output/lexicon_test.bin") == true);

    lexicon_entry le;
    bool res;
    res = lexicon.search("prova1", le);
    REQUIRE(res == true);
    REQUIRE(le.doc_freq == 1);
    REQUIRE(le.docs_offset == 1);
    REQUIRE(le.freqs_offset == 1);
    REQUIRE(le.max_score == 1);

    res = lexicon.search("prova2", le);
    REQUIRE(res == true);
    REQUIRE(le.doc_freq == 2);
    REQUIRE(le.docs_offset == 2);
    REQUIRE(le.freqs_offset == 2);
    REQUIRE(le.max_score == 2);

    res = lexicon.search("prova3", le);
    REQUIRE(res == true);
    REQUIRE(le.doc_freq == 3);
    REQUIRE(le.docs_offset == 3);
    REQUIRE(le.freqs_offset == 3);
    REQUIRE(le.max_score == 3);

    res = lexicon.search("prova4", le);
    REQUIRE(res == false);
}
