#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "MSMARCO-Search-Engine/merging.hpp"
#include "MSMARCO-Search-Engine/model.hpp"

TEST_CASE("PostingList1", "next") {
    term_entry te;
    te.term = "prova";

    for (int i=0; i<10; i++) {
        te.posting_list.push_back(std::make_pair(i, i*2));
    }
    std::ofstream out("../../output/test.bin", std::ios::binary | std::ios::out | std::ios::app);
    unsigned int offset = out.tellp();
    write_inverted_index_record_compressed(out, te);
    out.close();

    posting_list *pl = new posting_list();
    pl->n_skip_pointers = (unsigned int) ceil(sqrt(te.posting_list.size()));
    pl->f1.open("../../output/test.bin", std::ios::binary | std::ios::in);
    pl->openList(offset);

    for (auto it = te.posting_list.begin(); it != te.posting_list.end(); it++) {
        REQUIRE((it->first == pl->getDocId() && it->second == pl->getFreq()));
        pl->next();
    }
    REQUIRE(pl->getDocId() == std::numeric_limits<unsigned int>::max());
}
