/*
 * io 操作头文件。
 * 创建于 2022年7月2日。
 */

#pragma once

#include <sys/types.h>

extern "C" uint8_t io_in_byte(uint16_t port);
extern "C" uint16_t io_in_word(uint16_t port);

extern "C" void io_out_byte(uint16_t port, uint8_t data);
extern "C" void io_out_word(uint16_t port, uint16_t data);
