#ifndef LINKER_H
#define LINKER_H

#include <cstddef>
#include <vector>
#include <unordered_map>

enum class Visibility : bool {
    Global, Local,
};

inline std::ostream& operator<<(std::ostream& os, Visibility vis)
{
    switch (vis) {
    case Visibility::Global:
        return os << "global";
    case Visibility::Local:
        return os << "local";
    }
}

enum class Segment : bool {
    Text, Data,
};

inline std::ostream& operator<<(std::ostream& os, Segment seg)
{
    switch (seg) {
    case Segment::Text:
        return os << "text";
    case Segment::Data:
        return os << "data";
    }
}

enum class Instruction : uint8_t {
    JAL, J, LUI, ORI,
};

inline std::ostream& operator<<(std::ostream& os, Instruction inst)
{
    switch (inst) {
    case Instruction::JAL:
        return os << "jal";
    case Instruction::J:
        return os << "j";
    case Instruction::LUI:
        return os << "lui";
    case Instruction::ORI:
        return os << "ori";
    }
}

struct SymbolInfo {
    uint32_t address;
    Segment segment;

    SymbolInfo(uint32_t address = {}, Segment segment = {}) // NOLINT
            : address(address), segment(segment) { }
};

struct RelocationInfo {
    uint32_t address;
    Instruction instruction;
    std::string scope;
    std::string label;

    RelocationInfo(
            uint32_t address,
            const std::string& inst,
            const std::string& scope,
            const std::string& label
    )
            : address(address), scope(scope), label(label)
    {
        if (inst == "jal")
            instruction = Instruction::JAL;
        else if (inst == "j")
            instruction = Instruction::J;
        else if (inst == "lui")
            instruction = Instruction::LUI;
        else if (inst == "ori")
            instruction = Instruction::ORI;
        else {
            throw std::runtime_error{"bad instruction argument"};
        }
    }
};

struct Offset {
    std::string name;
    uint32_t text_offset;
    uint32_t data_offset;
    std::unordered_map<std::string, SymbolInfo> locals;

    Offset(std::string name = {}, uint32_t text_offset = {}, uint32_t data_offset = {}) // NOLINT
            : name(std::move(name)), text_offset(text_offset), data_offset(data_offset) { }
};

union data_t {
    uint32_t u;
    uint8_t b[4];
};



struct Linker {
    std::vector<uint32_t> text_segment;
    std::vector<data_t> data_segment;
    std::vector<uint32_t> linked;
    int text_start;
    int data_start;

    // Symbol offset
    std::vector<Offset> offsets;

    std::unordered_map<std::string, SymbolInfo> globals;
    std::vector<RelocationInfo> relocation_info;

    int link(int argc, char** argv);
    std::stringstream preprocess(std::istream& input);
    void read_file(std::istream& input);
    void read_text_line(std::istream& input);
    void read_data_line(std::istream& input);
    void read_symbol_line(std::istream& input);
    void read_relocation_line(std::istream& input);
    void relocate_references();
    void resolve(SymbolInfo&, RelocationInfo&);
};

#endif //LINKER_H
