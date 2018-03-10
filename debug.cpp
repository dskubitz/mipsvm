#include "debug.h"
#include "mips.h"

const char* reg_str(int r)
{
    switch (r) {
    case R_$zero: return "$zero";
    case R_$at: return "$at";
    case R_$v0: return "$v0";
    case R_$v1: return "$v1";
    case R_$a0: return "$a0";
    case R_$a1: return "$a1";
    case R_$a2: return "$a2";
    case R_$a3: return "$a3";
    case R_$t0: return "$t0";
    case R_$t1: return "$t1";
    case R_$t2: return "$t2";
    case R_$t3: return "$t3";
    case R_$t4: return "$t4";
    case R_$t5: return "$t5";
    case R_$t6: return "$t6";
    case R_$t7: return "$t7";
    case R_$s0: return "$s0";
    case R_$s1: return "$s1";
    case R_$s2: return "$s2";
    case R_$s3: return "$s3";
    case R_$s4: return "$s4";
    case R_$s5: return "$s5";
    case R_$s6: return "$s6";
    case R_$s7: return "$s7";
    case R_$t8: return "$t8";
    case R_$t9: return "$t9";
    case R_$k0: return "$k0";
    case R_$k1: return "$k1";
    case R_$gp: return "$gp";
    case R_$sp: return "$sp";
    case R_$fp: return "$fp";
    case R_$ra: return "$ra";
    default: return "?";
    }
}
