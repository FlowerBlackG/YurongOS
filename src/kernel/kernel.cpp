/*
 * 系统内核。
 * 创建于 2022年7月1日
 */

#include <sys/types.h>
#include <yros/io.h>
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
    uint8_t data = io_in_byte(CRT_DATA_REG);
    uint16_t a = 1;
}
