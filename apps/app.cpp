#include "MSMARCO-Search-Engine/parsing.hpp"
#include "MSMARCO-Search-Engine/merging.hpp"
#include "MSMARCO-Search-Engine/query.hpp"

#include <cli/cli.h>
#include <cli/clifilesession.h>
#include <Porter2.hpp>
#include <regex>

std::unordered_set<std::string> stopwords;
bool loaded_data = false;

void Clear()
{
#if defined _WIN32
    system("cls");
#elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
    system("clear");
#elif defined (__APPLE__)
    system("clear");
#endif
}

bool load_stopwords(std::unordered_set<std::string>& stopwords, std::string& filename) {
    std::ifstream filestream(filename);
    if (filestream.fail()) {
        std::cout << "Fail stopwords read!" << std::endl;
        return false;
    }

    std::string word;
    while (std::getline(filestream, word))
        stopwords.insert(word);

    filestream.close();
    return true;
}

static void parse(std::ostream& out, int block_size) {
    Clear();
    unsigned int n_threads = std::thread::hardware_concurrency() - 1;
    const char* docfile = "../../collection.tar.gz";
    const char* stopwords = "../../stopwords.txt";
    parse(docfile, block_size, true, stopwords, n_threads);
}

static void merge(std::ostream& out) {
    Clear();
    unsigned int n_blocks = 0;
    while (boost::filesystem::exists("../tmp/intermediate_" + std::to_string(n_blocks + 1))) {
        n_blocks++;
    }
    merge_blocks(n_blocks);
    loaded_data = false;
}

static void query(std::ostream& out, std::string& query, int mode, int k) {
    if (!loaded_data) {
        if ((!init_data_structures()) || (!load_stopwords(stopwords, std::string("../../stopwords.txt")))) {
            std::cout << "Failed to load data structures.\n";
            return;
        }
        loaded_data = true;
    }
    Clear();
    std::cout << query << "\n\n";
    if (mode < 3) {
        std::vector<std::string> query_terms;
        std::unordered_map<std::string, int> tokens;

        tokenize(query, true, stopwords, tokens);
        for (auto kv : tokens) {
            query_terms.push_back(kv.first);
        }
        execute_query(query_terms, mode, k);
    }
    else {
        out << "Wrong query type!\n";
    }
}

static void eval(std::ostream& out, int mode, int k, std::string& queriesfile) {
    if (!loaded_data) {
        if ((!init_data_structures()) || (!load_stopwords(stopwords, std::string("../../stopwords.txt")))) {
            std::cout << "Failed to load data structures.\n";
            return;
        }
        loaded_data = true;
    }
    Clear();
    if (mode < 3) {
        std::string topics = "../../evaluation/" + queriesfile;
        std::string results = "../../output/" + queriesfile + ".test";
        query_evaluation(topics, results, stopwords, mode, k);
    }
    else {
        out << "Wrong query type!\n";
    }
}

int main(int argc, char* argv[]){
    auto rootMenu = std::make_unique<cli::Menu>("MSMARCO");
    rootMenu->Insert(
        "q",
        query,
        "\n    <------------------------------------------>\n"
        "    <string> -> Query text: \"example of a query\"\n\n"
        "    <int> -> Type of query :\n"
        "    0 : CONJUNCTIVE_MODE\n"
        "    1 : DISJUNCTIVE_MODE\n"
        "    2 : DISJUNCTIVE_MODE_MAX_SCORE\n\n"
        "    <int> -> Top k results\n"
        "    <------------------------------------------>\n");
    rootMenu->Insert(
        "e",
        eval,
        "\n    <------------------------------------------>\n"
        "    Evaluation of the system, create the results.test file\n"
        "    <int> -> Type of query :\n"
        "    0 : CONJUNCTIVE_MODE\n"
        "    1 : DISJUNCTIVE_MODE\n"
        "    2 : DISJUNCTIVE_MODE_MAX_SCORE\n\n"
        "    <int> -> Top k results\n\n"
        "    <string> -> File name\n"
        "    <------------------------------------------>\n");
    auto subMenu = std::make_unique<cli::Menu>("build_index");
    subMenu->Insert(
        "p",
        parse,
        "Parsing the documents list and generate the intermediate posting lists\n"
        "         <------------------------------------------>\n"
        "         <int> -> Size of the intermediate blocks\n"
        "         <------------------------------------------>\n");
    subMenu->Insert(
        "m",
        merge,
        "Merging of the itermediate posting lists and create the compressed inverted index");


    rootMenu->Insert(std::move(subMenu));
    cli::Cli cli(std::move(rootMenu));

    cli.ExitAction([](auto& out) { Clear(); out << "Goodbye.\n"; });

    cli::CliFileSession input(cli);
    input.Start();

    return 0;
}