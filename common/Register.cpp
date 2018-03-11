#include "Register.h"
#include <ostream>

const std::unordered_map<std::string, Register> registers{
        {"$zero", Register::ZERO},
        {"$0",    Register::ZERO},
        {"$at",   Register::AT},
        {"$1",    Register::AT},
        {"$v0",   Register::V0},
        {"$2",    Register::V0},
        {"$v1",   Register::V1},
        {"$3",    Register::V1},
        {"$a0",   Register::A0},
        {"$4",    Register::A0},
        {"$a1",   Register::A1},
        {"$5",    Register::A1},
        {"$a2",   Register::A2},
        {"$6",    Register::A2},
        {"$a3",   Register::A3},
        {"$7",    Register::A3},
        {"$t0",   Register::T0},
        {"$8",    Register::T0},
        {"$t1",   Register::T1},
        {"$9",    Register::T1},
        {"$t2",   Register::T2},
        {"$10",   Register::T2},
        {"$t3",   Register::T3},
        {"$11",   Register::T3},
        {"$t4",   Register::T4},
        {"$12",   Register::T4},
        {"$t5",   Register::T5},
        {"$13",   Register::T5},
        {"$t6",   Register::T6},
        {"$14",   Register::T6},
        {"$t7",   Register::T7},
        {"$15",   Register::T7},
        {"$s0",   Register::S0},
        {"$16",   Register::S0},
        {"$s1",   Register::S1},
        {"$17",   Register::S1},
        {"$s2",   Register::S2},
        {"$18",   Register::S2},
        {"$s3",   Register::S3},
        {"$19",   Register::S3},
        {"$s4",   Register::S4},
        {"$20",   Register::S4},
        {"$s5",   Register::S5},
        {"$21",   Register::S5},
        {"$s6",   Register::S6},
        {"$22",   Register::S6},
        {"$s7",   Register::S7},
        {"$23",   Register::S7},
        {"$t8",   Register::T8},
        {"$24",   Register::T8},
        {"$t9",   Register::T9},
        {"$25",   Register::T9},
        {"$k0",   Register::K0},
        {"$26",   Register::K0},
        {"$k1",   Register::K1},
        {"$27",   Register::K1},
        {"$gp",   Register::GP},
        {"$28",   Register::GP},
        {"$sp",   Register::SP},
        {"$29",   Register::SP},
        {"$fp",   Register::FP},
        {"$30",   Register::FP},
        {"$ra",   Register::RA},
        {"$31",   Register::RA},
};

std::ostream& operator<<(std::ostream& os, Register reg)
{
    switch (reg) {
    case Register::ZERO:
        return os << "$zero";
    case Register::AT:
        return os << "$at";
    case Register::V0:
        return os << "$v0";
    case Register::V1:
        return os << "$v1";
    case Register::A0:
        return os << "$a0";
    case Register::A1:
        return os << "$a1";
    case Register::A2:
        return os << "$a2";
    case Register::A3:
        return os << "$a3";
    case Register::T0:
        return os << "$t0";
    case Register::T1:
        return os << "$t1";
    case Register::T2:
        return os << "$t2";
    case Register::T3:
        return os << "$t3";
    case Register::T4:
        return os << "$t4";
    case Register::T5:
        return os << "$t5";
    case Register::T6:
        return os << "$t6";
    case Register::T7:
        return os << "$t7";
    case Register::S0:
        return os << "$s0";
    case Register::S1:
        return os << "$s1";
    case Register::S2:
        return os << "$s2";
    case Register::S3:
        return os << "$s3";
    case Register::S4:
        return os << "$s4";
    case Register::S5:
        return os << "$s5";
    case Register::S6:
        return os << "$s6";
    case Register::S7:
        return os << "$s7";
    case Register::T8:
        return os << "$t8";
    case Register::T9:
        return os << "$t9";
    case Register::K0:
        return os << "$k0";
    case Register::K1:
        return os << "$k1";
    case Register::GP:
        return os << "$gp";
    case Register::SP:
        return os << "$sp";
    case Register::FP:
        return os << "$fp";
    case Register::RA:
        return os << "$ra";
    }
}