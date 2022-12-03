#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "MSMARCO-Search-Engine/disk_hash_map.hpp"
#include "MSMARCO-Search-Engine/model.hpp"

TEST_CASE("DiskHashMap1", "create") {
    DiskHashMap lexicon;
    
    REQUIRE(lexicon.create("../../output/lexicon_test.bin", 10000) == true);

    std::size_t h1 = std::hash<std::string>{}("prova");

    unsigned int index = h1 % 10000;
    unsigned int offset = index * sizeof(DiskHashMap::HashMapEntry);    // Offset where the entry will be written

    DiskHashMap::HashMapEntry e = lexicon.getEntryByOffset(offset);
    REQUIRE(e.isEmpty() == true);
    REQUIRE(e.next == 0);
}

TEST_CASE("DiskHashMap2", "insert") {
    DiskHashMap lexicon;
    REQUIRE(lexicon.create("../../output/lexicon_test.bin", 1) == true);

    lexicon_entry le1 = {1, 1, 1, 1};
    lexicon.insert("prova1", le1);

    std::size_t h1 = std::hash<std::string>{}("prova1");
    unsigned int index = h1 % 1;
    DiskHashMap::HashMapEntry he1 = lexicon.getEntryByOffset(index * sizeof(DiskHashMap::HashMapEntry));

    REQUIRE(!strcmp(he1.key, "prova1"));
    REQUIRE(he1.le.docs_offset == 1);
    REQUIRE(he1.next == 0);

    lexicon_entry le2 = {2, 2, 2, 2};
    lexicon.insert("prova2", le2);
    he1 = lexicon.getEntryByOffset(index * sizeof(DiskHashMap::HashMapEntry));

    DiskHashMap::HashMapEntry he2 = lexicon.getEntryByOffset(1 * sizeof(DiskHashMap::HashMapEntry));
    REQUIRE(!strcmp(he2.key, "prova2"));
    REQUIRE(he2.le.docs_offset == 2);
    REQUIRE(he2.next == 0);
    REQUIRE(he1.next == 1 * sizeof(DiskHashMap::HashMapEntry));

    lexicon_entry le3 = {3, 3, 3, 3};
    lexicon.insert("prova3", le3);
    DiskHashMap::HashMapEntry he3 = lexicon.getEntryByOffset(2 * sizeof(DiskHashMap::HashMapEntry));
    he2 = lexicon.getEntryByOffset(1 * sizeof(DiskHashMap::HashMapEntry));
    REQUIRE(!strcmp(he3.key, "prova3"));
    REQUIRE(he3.le.docs_offset == 3);
    REQUIRE(he3.next == 0);
    REQUIRE(he2.next == 2 * sizeof(DiskHashMap::HashMapEntry));
}

TEST_CASE("DiskHashMap3", "search") {
    DiskHashMap lexicon;
    lexicon.create("../../output/lexicon_test.bin", 10);

    lexicon_entry le1 = {1, 1, 1, 1};
    lexicon.insert("prova1", le1);
    lexicon_entry le2 = {2, 2, 2, 2};
    lexicon.insert("prova2", le2);
    lexicon_entry le3 = {3, 3, 3, 3};
    lexicon.insert("prova3", le3);

    lexicon_entry le;
    bool res;
    res = lexicon.search("prova1", le);
    REQUIRE(res == true);
    REQUIRE(le.doc_freq == 1);
    REQUIRE(le.docs_offset == 1);
    REQUIRE(le.freqs_offset == 1);
    REQUIRE(le.max_score == 1);
}