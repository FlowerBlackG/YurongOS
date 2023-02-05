/*
 * ASCII 特殊字符定义。
 * 创建于 2022年7月12日。
 * 
 * 参考：
 *   https://www.asciitable.com/
 */

#pragma once
#include <lib/sys/types.h>

class AsciiChar {
public:
    static const uint8_t NUL = 0;
    static const uint8_t SOH = 1;
    static const uint8_t STX = 2;
    static const uint8_t ETX = 3;
    static const uint8_t EOT = 4;
    static const uint8_t ENQ = 5;
    static const uint8_t ACK = 6;
    static const uint8_t BEL = 7;
    static const uint8_t BS = 8;
    static const uint8_t TAB = 9;

    /** NL line feed, new line (n). */
    static const uint8_t LF = 10;
    static const uint8_t VT = 11;
    static const uint8_t FF = 12;

    /** carriage return (r). */
    static const uint8_t CR = 13;
    static const uint8_t SO = 14;
    static const uint8_t SI = 15;
    static const uint8_t DLE = 16;
    static const uint8_t DC1 = 17;
    static const uint8_t DC2 = 18;
    static const uint8_t DC3 = 19;
    static const uint8_t DC4 = 20;
    static const uint8_t NAK = 21;
    static const uint8_t SYN = 22;
    static const uint8_t ETB = 23;
    static const uint8_t CAN = 24;
    static const uint8_t EM = 25;
    static const uint8_t SUB = 26;
    static const uint8_t ESC = 27;
    static const uint8_t FS = 28;
    static const uint8_t GS = 29;
    static const uint8_t RS = 30;
    static const uint8_t US = 31;

    /** 可见字符起始位置。含空格。 */
    static const uint8_t START_OF_VISIBLE_CHARS = 32;
    static const uint8_t END_OF_VISIBLE_CHARS = 126;
};
