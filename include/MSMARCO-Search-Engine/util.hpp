#pragma once

#include <list>

struct term_entry {
    int block_id;
    std::string term;
    std::list<std::pair<int, int>> posting_list;
};