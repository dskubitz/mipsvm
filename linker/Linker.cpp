#include <iostream>
#include <fstream>
#include <sstream>
#include "Linker.h"

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << "file(s) ...\n";
        return 1;
    }
    return Linker{}.link(argc, argv);
}

int Linker::link(int argc, char** argv)
{
    for (int i = 1; i < argc; ++i) {
        const char* arg = argv[i];
        std::cout << arg << '\n';
        std::ifstream input(arg);
        if (input.is_open()) {
            add_object_file(input);
        }
        else {
            std::cerr << "failed to open file " << arg << ", aborting...\n";
            return 2;
        }
    }
    return 0;
}

void Linker::add_object_file(std::istream& input)
{
    for (std::string word; input >> word;) {
        if (word == ".text") {
            read_text_segment(input);
        }
        else if (word == ".data");
        else if (word == ".symbol");
        else if (word == ".relocation");
    }
}

void Linker::read_text_segment(std::istream& input)
{
}
