#ifndef LINKER_H
#define LINKER_H

#include <cstddef>
#include <vector>
#include <unordered_map>

struct Linker {
    std::vector<uint32_t> text_segment;
    std::vector<uint8_t> data_segment;

    // Symbol offset
    uint32_t text_offset;
    uint32_t data_offset;

    std::unordered_map<std::string, uint32_t> data_labels;
    std::unordered_map<std::string, uint32_t> text_labels;
    std::vector<std::tuple<uint32_t, std::string, std::string>> relocation_info;


    int link(int argc, char** argv);
    std::stringstream preprocess(std::istream& input);
    void read_file(std::istream& input);
    void read_text_line(std::istream& input);
    void read_data_line(std::istream& input);
    void read_symbol_line(std::istream& input);
    void read_relocation_line(std::istream& input);
};


#endif //LINKER_H
