#include "MSMARCO-Search-Engine/query.hpp"

int main(int argc, char* argv[])
{
    if (!init_data_structures()) {
        std::cout << "Failed to load data structures.\n";
    }
    std::vector<std::string> query_terms = { "aziz" , "hashim"};
    std::cout << "Disjunctive Query \n";
    if (execute_query(query_terms, DISJUNCTIVE_MODE, 10))
        std::cout << "SUCCESS\n";
    std::cout << "Disjunctive Query with Max Score\n";
    if (execute_query(query_terms, DISJUNCTIVE_MODE_MAX_SCORE, 10))
        std::cout << "SUCCESS\n";
    std::cout << "Conjunctive Query\n";
    if (execute_query(query_terms, CONJUNCTIVE_MODE, 10))
        std::cout << "SUCCESS\n";
}