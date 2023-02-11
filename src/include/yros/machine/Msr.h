/*

    Model Specific Registers
    创建于 2023年2月5日 江西省上饶市玉山县

*/

#pragma once

#include <lib/sys/types.h>

namespace Msr {

    const int SYSCALL_STAR = 0xC0000081;
    const int SYSCALL_LSTAR = 0xC0000082;
    const int SYSCALL_CSTAR = 0xC0000083;
    const int SYSCALL_SFMASK = 0xC0000084;

    const int FS_BASE = 0xC0000100;
    const int GS_BASE = 0xC0000101;
    const int KERNEL_GS_BASE = 0xC0000102;

    inline uint64_t read(uint32_t msr) {
        uint32_t low, high;

        __asm (
            "rdmsr"
            : "=a" (low), "=d" (high)
            : "c" (msr)
        );

        return ((high << 16ULL) << 16ULL) | low;
    }

    inline void write(uint32_t msr, uint64_t value) {
        uint32_t low = value & 0xffffffff;
        uint32_t high = (value >> 16) >> 16;
        
        __asm (
            "wrmsr"
            :
            : "a" (low), "d" (high), "c" (msr)
        );
    }

};
