#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "MSMARCO-Search-Engine/merging.hpp"
#include "MSMARCO-Search-Engine/model.hpp"

TEST_CASE("Compressing1", "eq") {
    std::vector<uint8_t> bytes;
    VBencode(67822, bytes);
    std::ofstream out("../../output/test1.bin", std::ios::binary | std::ios::out | std::ios::app);
    unsigned int num_bytes_written = 0;

    for (std::vector<uint8_t>::iterator it = bytes.begin(); it != bytes.end(); it++) {
        out.write(reinterpret_cast<const char*>(&(*it)), 1);
        num_bytes_written++;
    }
    std::ifstream instream("../../output/test1.bin", std::ios::binary | std::ios::in);

    unsigned int n_bytes = 0;
    unsigned int res = VBdecode(instream, n_bytes);

    REQUIRE(res == 67822);
    REQUIRE(n_bytes == num_bytes_written);
}