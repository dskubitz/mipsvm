#include <fstream>
#include <iostream>
#include "mips.h"

int main(int argc, char** argv)
{
    std::ifstream input("test");
    if (!input.is_open())
        return 1;

    size_t mem_size = 30, stack_seg = 30, data_seg = 20, text_seg = 0;
    std::string s1, s2;
    input >> s1 >> s2;
    text_seg = std::stoull(s2, nullptr, 16);
    input >> s1 >> s2;
    data_seg = std::stoull(s2, nullptr, 16);
    input >> s1 >> s2;
    mem_size = stack_seg = 2 * std::stoull(s2, nullptr, 16);
    std::cout << mem_size << ' ' << stack_seg << ' ' << data_seg << ' ' << text_seg << '\n';

    memory_t* mem = create_memory(mem_size, stack_seg, data_seg, text_seg);
    uint32_t* ptr = mem->data;

    for (std::string line; std::getline(input, line);) {
        if (line.empty()) continue;
        *ptr++ = std::stoul(line, nullptr, 16);
    }

    execute(mem);

    destroy_memory(mem);
    return 0;
}