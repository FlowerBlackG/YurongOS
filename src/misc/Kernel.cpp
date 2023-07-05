// SPDX-License-Identifier: MulanPSL-2.0

/*
 * 系统内核。
 * 创建于 2023年7月5日
 */

#include "./Kernel.h"
#include <lib/stdio.h>
#include <machine/X86Assembly.h>
#include <crt/CRT.h>

void Kernel::panic(const char* s) {
    x86asmCli();

    CRT::getInstance().write(s);

    while (true) {
        x86asmHlt();
    }
}

void Kernel::printLog(const char* format, ...) {
    char buf[256];
    va_list args;
    va_start(args, format);
    vsprintf(buf, format, args);
    CRT::getInstance().write(buf);
    va_end(args);
}

void Kernel::printLog(LogColor fg, const char* format, ...) {
    char buf[256];
    va_list args;
    va_start(args, format);
    vsprintf(buf, format, args);
    CRT::getInstance().write(
        buf,
        CRT::makeAttr(0, (uint8_t) fg, false, false)
    );
    va_end(args);
}

void Kernel::printLog(LogColor fg, LogColor bg, const char* format, ...) {
    char buf[256];
    va_list args;
    va_start(args, format);
    vsprintf(buf, format, args);CRT::getInstance().write(
        buf,
        CRT::makeAttr((uint8_t) bg, (uint8_t) fg, false, false)
    );
    va_end(args);
}

