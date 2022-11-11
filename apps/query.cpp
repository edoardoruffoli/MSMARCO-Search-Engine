#include "MSMARCO-Search-Engine/query.hpp"

int main(int argc, char* argv[])
{
    if (!init_data_structures()) {
        std::cout << "Failed to load data structures.\n";
    }
    std::vector<std::string> query_terms = {"applianc", "power"};
    if (execute_query(query_terms, CONJUNCTIVE_MODE))
        std::cout << "SUCCESS\n";
}
