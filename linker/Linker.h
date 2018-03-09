#ifndef LINKER_H
#define LINKER_H

#include <cstddef>
#include <vector>

struct Linker {
    std::vector<uint32_t> text_segment;
    std::vector<uint8_t> data_segment;
    int link(int argc, char** argv);
    void add_object_file(std::istream& input);
    void read_text_segment(std::istream& input);
};


#endif //LINKER_H
