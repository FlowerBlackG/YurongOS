/*

    RFlags 寄存器

    创建于 2023年2月12日 江西省上饶市玉山县

*/

#pragma once

#include <lib/sys/types.h>

struct RflagsRegister {
    uint64_t cf : 1; // bit 0
    uint64_t one : 1;
    uint64_t pf : 1;
    uint64_t zero0 : 1; // bit 3
    uint64_t af : 1;
    uint64_t zero1 : 1; // bit 5
    uint64_t zf : 1;
    uint64_t sf : 1;
    uint64_t tf : 1; // bit 8
    uint64_t interruptFlag : 1;
    uint64_t df : 1;
    uint64_t of : 1; // bit 11
    uint64_t iopl : 2; 
    uint64_t nt : 1;
    uint64_t zero2 : 1;
    uint64_t rf : 1; // bit 16
    uint64_t vm : 1;
    uint64_t ac : 1;
    uint64_t vif : 1;
    uint64_t vip : 1; // bit 20
    uint64_t id : 1;

    uint64_t reserved : 43;



    static const int TF_BIT = (1 << 8);
    static const int IF_BIT = (1 << 9);

} __packed;
