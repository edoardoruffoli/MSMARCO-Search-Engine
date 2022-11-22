#include "MSMARCO-Search-Engine/parsing.hpp"
#include "MSMARCO-Search-Engine/merging.hpp"
#include "MSMARCO-Search-Engine/query.hpp"

#include <cli/cli.h>
#include <cli/clifilesession.h>
#include <Porter2.hpp>

std::unordered_set<std::string> stopwords;

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
    size_t pos = 0;
    std::string token;
    std::string delimiter = " ";
    while ((pos = content.find(delimiter)) != std::string::npos) {
        token = content.substr(0, pos);
        content.erase(0, pos + delimiter.length());

        // Remove spaces
        remove(token.begin(), token.end(), ' ');

        // Remove punctuation
        token.erase(std::remove_if(token.begin(), token.end(), ispunct), token.end());

        // To lower case
        std::transform(token.begin(), token.end(), token.begin(), [](unsigned char c) {
            return std::tolower(c);
            });

        // If token is empty continue
        if (!token.size())
            continue;

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
    if ((!init_data_structures()) || (!load_stopwords(stopwords, std::string("../../stopwords.txt")))) {
        std::cout << "Failed to load data structures.\n";
        return 1;
    }
    auto rootMenu = std::make_unique<cli::Menu>("MSMARCO-Search-Engine");
    rootMenu->Insert(
        "p",
        parse,
        "Parsing the documents list and generate the intermediate posting lists\n"
        "         <------------------------------------------>\n"
        "         <int> -> Size of the intermediate blocks\n"
        "         <------------------------------------------>\n");
    rootMenu->Insert(
        "m",
        merge,
        "Merging of the itermediate posting lists and create the compressed inverted index");

    auto subMenu = std::make_unique<cli::Menu>("queries");
    subMenu->Insert(
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

    rootMenu->Insert(std::move(subMenu));
    cli::Cli cli(std::move(rootMenu));

    cli.ExitAction([](auto& out) { Clear(); out << "Goodbye.\n"; });

    cli::CliFileSession input(cli);
    input.Start();

    return 0;
}