/*
 * 系统内核。
 * 创建于 2022年7月1日
 */

#include <sys/types.h>
#include <yros/IO.h>
#include <yros/kernel.h>
#include <crt.h>

/**
 * 内核初始化。
 * 加入 extern "C" 以防止 C++ 编译器将函数重命名，导致链接失败。
 * 
 */
extern "C" void kernel_init() {
    char* video = (char*) 0xb8000;
    video[0]='z';

    IO::outByte(CRT_ADDR_REG, CRT_REG_CURSOR_HIGH);
    uint16_t pos = IO::inByte(CRT_DATA_REG) << 8;
    IO::outByte(CRT_ADDR_REG, CRT_REG_CURSOR_LOW);
    pos |= IO::inByte(CRT_DATA_REG);

    uint16_t a = 10;

    IO::outByte(CRT_ADDR_REG, CRT_REG_CURSOR_HIGH);
    IO::outByte(CRT_DATA_REG, 0);
    
    IO::outByte(CRT_ADDR_REG, CRT_REG_CURSOR_LOW);
    IO::outByte(CRT_DATA_REG, 0);

    uint16_t ab = 10;
}
