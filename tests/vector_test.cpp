#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "MSMARCO-Search-Engine/disk_vector.hpp"
#include "MSMARCO-Search-Engine/model.hpp"

TEST_CASE("DiskVector1", "create") {
    DiskVector doc_table;
    
    REQUIRE(doc_table.create("../../output/doc_table_test.bin") == true);

    std::vector<doc_table_entry> entries;
    doc_table_entry de;
    for (int i=0; i<10000; i++) {
        std::string tmp = std::to_string(i);
        strcpy(de.doc_no, tmp.c_str()); 
        de.doc_len = i;
        entries.push_back(de);
    }

    doc_table.insert(entries);
    doc_table.insert(entries);

    bool res;

    res = doc_table.getEntryByIndex(500, de);
    REQUIRE(res == true);
    REQUIRE(!strcmp(de.doc_no, "500"));
    REQUIRE(de.doc_len == 500);

    res = doc_table.getEntryByIndex(10500, de);
    REQUIRE(res == true);
    REQUIRE(!strcmp(de.doc_no, "500"));
    REQUIRE(de.doc_len == 500);

    doc_table.close();
}

TEST_CASE("DiskVector2", "open") {
    DiskVector doc_table;
    
    REQUIRE(doc_table.open("../../output/doc_table_test.bin") == true);

    doc_table_entry de;
    bool res;

    res = doc_table.getEntryByIndex(500, de);
    REQUIRE(res == true);
    REQUIRE(!strcmp(de.doc_no, "500"));
    REQUIRE(de.doc_len == 500);

    res = doc_table.getEntryByIndex(10500, de);
    REQUIRE(res == true);
    REQUIRE(!strcmp(de.doc_no, "500"));
    REQUIRE(de.doc_len == 500);

    doc_table.close();
}