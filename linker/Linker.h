#ifndef LINKER_H
#define LINKER_H

#include <cstddef>
#include <vector>

struct ObjectFile {
    std::vector<uint32_t> text_segment;
    std::vector<uint8_t> data_segment;

};


#endif //LINKER_H
