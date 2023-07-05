// SPDX-License-Identifier: MulanPSL-2.0

/*
 * 内核头文件。
 * 创建于 2022年7月1日
 * 
 * 实现见 main.cpp
 */

#pragma once

#include <lib/sys/types.h>

namespace Kernel {


void panic(const char* s);
void main();

enum class LogColor : uint8_t {
    WHITE = 7,
    YELLOW = 6,
    PURPLE = 5,
    RED = 4,
    CYAN = 3,
    GREEN = 2,
    BLUE = 1,
    BLACK = 0,

    H_WHITE = 7 | 8,
    H_YELLOW = 6 | 8,
    H_PURPLE = 5 | 8,
    H_RED = 4 | 8,
    H_CYAN = 3 | 8,
    H_GREEN = 2 | 8,
    H_BLUE = 1 | 8,
    H_BLACK = 0 | 8
};

void printLog(const char* format, ...);
void printLog(LogColor fg, const char* format, ...);
void printLog(LogColor fg, LogColor bg, const char* format, ...);

};
