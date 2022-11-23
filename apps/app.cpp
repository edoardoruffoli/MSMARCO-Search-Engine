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

void query_tokenize(std::string& content, std::vector<std::string>& tokens) {

    // How to deal with empty page, malformed lines, malformed characters?
    std::regex re("[ \t]");
    //the '-1' is what makes the regex split (-1 := what was not matched)
    std::sregex_token_iterator first{ content.begin(), content.end(), re, -1 }, last;
    std::vector<std::string> v{ first, last };

    for (auto token : v) {
        if (!token.size())
            continue;

        // Remove punctuation
        token.erase(std::remove_if(token.begin(), token.end(), [](unsigned char c) {
            return std::ispunct(c);
            }), token.end());

        if (!token.size())
            continue;

        // To lower case
        std::transform(token.begin(), token.end(), token.begin(), [](unsigned char c) {
            return std::tolower(c);
            });

        if (stopwords.find(token) != stopwords.end()) {
            continue;
        }
        token = porter2::Stemmer{}.stem(token);
        tokens.push_back(token);
    }
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
    if (mode < 4) {
        std::vector<std::string> query_terms;
        query_tokenize(query, query_terms);
        execute_query(query_terms, mode, k);
    }
    else {
        out << "Wrong query type!\n";
    }
}

int main(int argc, char* argv[]){
    auto rootMenu = std::make_unique<cli::Menu>("MSMARCO-Search-Engine");
    rootMenu->Insert(
        "q",
        query,
        "\n    <------------------------------------------>\n"
        "    <string> -> Query text: \"example of a query\"\n\n"
        "    <int> -> Type of query :\n"
        "    0 : CONJUNCTIVE_MODE\n"
        "    1 : DISJUNCTIVE_MODE\n"
        "    2 : CONJUNCTIVE_MODE_MAX_SCORE\n"
        "    3 : DISJUNCTIVE_MODE_MAX_SCORE\n\n"
        "    <int> -> Top k results\n"
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