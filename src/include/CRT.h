/*
 * CRT 显示器控制头文件。
 * 创建于 2022年7月2日。
 */

#pragma once

#include <sys/types.h>

class CRT {
private:
    CRT();

public:
    /**
     * 清空屏幕。
     */
    void clear();
    static CRT& getInstance() {
        return CRT::instance;
    }

private:
    static const int ADDR_REG = 0x3d4;
    static const int DATA_REG = 0x3d5;

    struct Reg {
        static const int MEM_START_ADDR_HIGH = 0xc;
        static const int MEM_START_ADDR_LOW = 0xd;
        static const int CURSOR_HIGH = 0xe;
        static const int CURSOR_LOW = 0xf;
    };

    /**
     * CRT 显存位置开始地址。
     */
    static const int MEMORY_BASE = 0xb8000;

    /**
     * CRT 显存大小。32KB。
     */
    static const int MEMORY_SIZE = 32 * 1024;

    /**
     * CRT 单页空间。4000字节。
     */
    static const int PAGE_MEMORY_SIZE = 4000;

    /**
     * 屏幕页数。
     */
    static const int PAGES = CRT::MEMORY_SIZE / CRT::PAGE_MEMORY_SIZE;

    /**
     * 屏幕字符列数。
     */
    static const int COLS = 80;

    /**
     * 字符样式。
     */
    struct CharAttr {
        /**
         * 闪烁位。
         */
        static const uint8_t BLINK_BIT = 1 << 7;

        static const uint8_t BACKGROUND_RED_BIT = 1 << 6;
        static const uint8_t BACKGROUND_GREEN_BIT = 1 << 5;
        static const uint8_t BACKGROUND_BLUE_BIT = 1 << 4;

        /**
         * 高亮位。
         */
        static const uint8_t HIGH_LIGHT_BIT = 1 << 3;

        static const uint8_t FOREGROUND_RED_BIT = 1 << 2;
        static const uint8_t FOREGROUND_GREEN_BIT = 1 << 1;
        static const uint8_t FOREGROUND_BLUE_BIT = 1 << 0;

        static const uint8_t BACKGROUND_OFFSET = 4;

        static const uint8_t WHITE = 7;
        static const uint8_t YELLOW = 6;
        static const uint8_t PURPLE = 5;
        static const uint8_t RED = 4;
        static const uint8_t CYAN = 3;
        static const uint8_t GREEN = 2;
        static const uint8_t BLUE = 1;
        static const uint8_t BLACK = 0;
    };

private:
    /**
     * 单例对象。
     */
    static CRT instance;

    uint8_t cursorX;
    uint8_t cursorY;

    /**
     * 字符样式。
     * 0x07: 黑底白字，无高亮，无闪烁。
     */
    uint8_t charAttr = 0x07;

    /**
     * 当前屏幕内容内存起始位置。
     */
    size_t currentMemoryStart = CRT::MEMORY_BASE;

};
