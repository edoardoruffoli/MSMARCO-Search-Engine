#include "MSMARCO-Search-Engine/parsing.hpp"
#include "MSMARCO-Search-Engine/merging.hpp"
#include "MSMARCO-Search-Engine/query.hpp"
#include <Porter2.hpp>
#include <regex>

std::unordered_set<std::string> stopwords;
bool loaded_data = false;
const char* docfile = "../../collection.tar.gz";
const char* stopwordsFile = "../../stopwords.txt";

// Function prototypes
void clear();
void printMenu();
void printHelp();
void handleInsertQueryParam(int& mode, int& k);
void handleQuery(std::string& query, int mode, int k);
void handleParse(int block_num);
void handleMerge();
void handleEval(int mode, int k, std::string& queriesfile);

int main() {

    while (true) {
        printMenu();

        std::string command;
        std::cin >> command;

        if (command == "help") {
            clear();
            printHelp();
        }
        else if (command == "query") {
            int k;
            int mode;
            handleInsertQueryParam(mode, k);
            while (true) {
                std::string arg;
                std::cout << "Digit _exit to return to main menu, or\n" << std::endl;
                std::cout << "Enter the query:\n" << std::endl << ">";
                std::getline(std::cin, arg);
                if (arg == "_exit") {
                    clear();
                    break;
                }
                clear();
                handleQuery(arg, mode, k);
            }
        }
        else if (command == "parse") {
            int block_size;
            std::cout << "Enter the block size:" << std::endl << ">";
            std::cin >> block_size;
            while (!std::cin.good()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                clear();
                std::cout << "Enter the block size:" << std::endl << ">";
                std::cin >> block_size;
            }
            handleParse(block_size);
        }
        else if (command == "merge") {
            handleMerge();
        }
        else if (command == "eval") {
            int k;
            int mode;
            handleInsertQueryParam(mode, k);
            std::string arg;
            std::cout << "Enter file name:\n" << std::endl << ">";
            std::getline(std::cin, arg);
            clear();
            handleEval(mode, k, arg);
        }
        else if (command == "exit") {
            std::cout << "Goodbye.\n";
            break;
        }
        else {
            std::cout << "Invalid command. Type 'help' for a list of commands." << std::endl;
        }
    }

    return 0;
}

void clear() {
    #if defined _WIN32
    system("cls");
    #elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
    int i = system("clear");
    #endif
}

// Prints the main menu to the console
void printMenu() {
    std::cout << "Enter a command: \n" << ">";
}

// Prints a list of available commands to the console //DA FINIRE
void printHelp() {
    std::cout << "Available commands:" << std::endl;
    std::cout << "  help - display a list of commands" << std::endl;
    std::cout << "  query <int> <int> <string> - perform a query" << std::endl;
    std::cout << "  eval <int> <int> <string> - execute a queries dataset, saving the result file for trec_eval " << std::endl;
    std::cout << "  parse <int> - create the intermediate posting lists " << std::endl;
    std::cout << "  merge - merge intermediate posting lists to create the index" << std::endl;
    std::cout << "  exit - exit the program" << std::endl;
    std::cout << std::endl;
}

bool load_stopwords(std::unordered_set<std::string>& stopwords, const char* filename) {
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

void handleInsertQueryParam(int &mode, int &k) {
    std::cout << "Enter the query exectuon mode:\n"
        "    0 : CONJUNCTIVE_MODE\n"
        "    1 : DISJUNCTIVE_MODE\n"
        "    2 : DISJUNCTIVE_MODE_MAX_SCORE\n " << std::endl << ">";
    //print dei tre valori
    std::cin >> mode;
    while (!std::cin.good() || mode > 2) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        clear();
        std::cout << "Enter the query exectuon mode:\n"
            "    0 : CONJUNCTIVE_MODE\n"
            "    1 : DISJUNCTIVE_MODE\n"
            "    2 : DISJUNCTIVE_MODE_MAX_SCORE\n " << std::endl << ">";
        std::cin >> mode;
    }
    std::cout << "Select how many documents return:" << std::endl << ">";
    std::cin >> k;
    while (!std::cin.good()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        clear();
        std::cout << "Enter the number of relevant documents to return:" << std::endl << ">";
        std::cin >> k;
    }
    std::cin.ignore();
    clear();
}

// Performs a query with the given arguments
void handleQuery(std::string& query, int mode, int k) {
    if (!loaded_data) {
        if ((!init_data_structures()) || (!load_stopwords(stopwords, stopwordsFile))) {
            std::cout << "Failed to load data structures.\n";
            return;
        }
        loaded_data = true;
    }
    clear();
    std::cout << mode << " " << k << std::endl;
    std::cout << query << "\n\n";
    std::vector<std::string> query_terms;
    std::unordered_map<std::string, int> tokens;

    tokenize(query, true, stopwords, tokens);
    for (auto kv : tokens) {
        query_terms.push_back(kv.first);
    }
    execute_query(query_terms, mode, k);
}

void handleParse(int block_size) {
    clear();
    unsigned int n_threads = std::thread::hardware_concurrency() - 1;
    parse(docfile, block_size, true, stopwordsFile, n_threads);
}

void handleMerge() {
    clear();
    unsigned int n_blocks = 0;
    while (boost::filesystem::exists("../tmp/intermediate_" + std::to_string(n_blocks + 1))) {
        n_blocks++;
    }
    merge_blocks(n_blocks);
    loaded_data = false;
}

void handleEval(int mode, int k, std::string& queriesfile) {
    if (!loaded_data) {
        if ((!init_data_structures()) || (!load_stopwords(stopwords, stopwordsFile))) {
            std::cout << "Failed to load data structures.\n";
            return;
        }
        loaded_data = true;
    }
    clear();
    std::string topics = "../../evaluation/" + queriesfile;
    std::string results = "../../output/" + queriesfile + ".test";
    query_evaluation(topics, results, stopwords, mode, k);
}