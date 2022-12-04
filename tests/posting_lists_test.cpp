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
    std::ofstream out_docs("../../output/test_docs.bin", std::ios::binary | std::ios::out);
    std::ofstream out_freqs("../../output/test_freqs.bin", std::ios::binary | std::ios::out);

    unsigned int docs_offset = out_docs.tellp();
    unsigned int freqs_offset = out_freqs.tellp();

    write_inverted_index_record_compressed(out_docs, out_freqs, te);
    out_docs.close();
    out_freqs.close();

    posting_list *pl = new posting_list();
    pl->f_docs.open("../../output/test_docs.bin", std::ios::binary | std::ios::in);
    pl->f_freqs.open("../../output/test_freqs.bin", std::ios::binary | std::ios::in);
    pl->openList(docs_offset, freqs_offset, 10);

    for (auto it = te.posting_list.begin(); it != te.posting_list.end(); it++) {
        REQUIRE((it->first == pl->getDocId() && it->second == pl->getFreq()));
        pl->next();
    }
    REQUIRE(pl->getDocId() == std::numeric_limits<unsigned int>::max());
}
