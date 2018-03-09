#include <iostream>
#include <fstream>
#include "Linker.h"

int main(int argc, char** argv)
{
    std::ifstream input("main.o");
    std::string line;
    if (input.is_open()) {
        for (;std::getline(input, line); ) {
            std::cout << line << '\n';
        }
    }
}
