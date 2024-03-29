// SPDX-License-Identifier: MulanPSL-2.0

/*
 * io 操作头文件。
 * 创建于 2022年7月2日。
 */

#pragma once

#include <lib/sys/types.h>


/**
 * IO 操作封装类。底层是汇编代码。
 */
namespace io {


extern "C" uint8_t __io_in_byte(uint16_t port);
extern "C" uint16_t __io_in_word(uint16_t port);
extern "C" uint32_t __io_in_dword(uint16_t port);

extern "C" void __io_out_byte(uint16_t port, uint8_t data);
extern "C" void __io_out_word(uint16_t port, uint16_t data);
extern "C" void __io_out_dword(uint16_t port, uint32_t data);


uint8_t inline inByte(uint16_t port) {
    return __io_in_byte(port);
}

uint16_t inline inWord(uint16_t port) {
    return __io_in_word(port);
}

uint32_t inline inDWord(uint32_t port) {
    return __io_in_dword(port);
}

void inline outByte(uint16_t port, uint8_t data) {
    __io_out_byte(port, data);
}

void inline outWord(uint16_t port, uint16_t data) {
    __io_out_word(port, data);
}

void inline outDWord(uint16_t port, uint32_t data) {
    __io_out_dword(port, data);
}


}

