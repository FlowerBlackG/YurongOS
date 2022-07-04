/*
 * io 操作头文件。
 * 创建于 2022年7月2日。
 */

#pragma once

#include <sys/types.h>

extern "C" uint8_t __io_in_byte(uint16_t port);
extern "C" uint16_t __io_in_word(uint16_t port);

extern "C" void __io_out_byte(uint16_t port, uint8_t data);
extern "C" void __io_out_word(uint16_t port, uint16_t data);

/**
 * IO 操作封装类。底层是汇编代码。
 */
class IO {
public:
    static uint8_t inByte(uint16_t port) {
        return __io_in_byte(port);
    }

    static uint16_t inWord(uint16_t port) {
        return __io_in_word(port);
    }

    static void outByte(uint16_t port, uint8_t data) {
        __io_out_byte(port, data);
    }

    static void outWord(uint16_t port, uint16_t data) {
        __io_out_word(port, data);
    }

};
