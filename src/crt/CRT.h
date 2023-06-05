/*
 * CRT 显示器控制头文件。
 * 创建于 2022年7月2日。
 */

#pragma once
#include <lib/sys/types.h>

#include <memory/MemoryManager.h>

class CRT {
protected:
    CRT();

public:

    void init();

    /**
     * 清空屏幕。
     */
    void clear();

    /**
     * 移动光标。 
     */
    void moveCursor(uint8_t x, uint8_t y);

public:

    static CRT& getInstance() {
        return CRT::instance;
    }

protected:
    static const int ADDR_REG = 0x3d4;
    static const int DATA_REG = 0x3d5;

    struct Reg {
        static const int SCREEN_POS_HIGH = 0xc;
        static const int SCREEN_POS_LOW = 0xd;
        static const int CURSOR_HIGH = 0xe;
        static const int CURSOR_LOW = 0xf;
    };

    /**
     * CRT 显存位置开始地址。
     */
    static const long MEMORY_BASE = 0xb8000 + MemoryManager::ADDRESS_OF_PHYSICAL_MEMORY_MAP;

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

public:
    /**
     * 字符样式。
     */
    struct CharAttr {

        /*

            | BLK | BG_R | BG_G | BG_B | HL | FG_R | FG_G | FG_B |

        */

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

        static const uint8_t BACKGROUND_BITS_OFFSET = 4;

        struct Color {
            static const uint8_t WHITE = 7;
            static const uint8_t YELLOW = 6;
            static const uint8_t PURPLE = 5;
            static const uint8_t RED = 4;
            static const uint8_t CYAN = 3;
            static const uint8_t GREEN = 2;
            static const uint8_t BLUE = 1;
            static const uint8_t BLACK = 0;
        };
    }; // struct CharAttr


public:
    void setCharAttr(uint8_t attr);
    void setBackgroundColor(uint8_t color = CharAttr::Color::BLACK);

    /**
     * 设置前景颜色。
     * 
     * @param color 
     * @param blink 是否闪烁。1：是。0：否。2：翻转原有选项。3：维持原状。
     * @param highlight 
     */
    void setForegroundColor(
        uint8_t color = CharAttr::Color::WHITE,
        uint8_t blink = 2,
        uint8_t highlight = 2
    );

    void scrollUp(uint16_t lines = 1);

    void putchar(uint8_t ch);
    size_t write(const char* str, size_t len = -1);

protected:
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
     * 当前屏幕内容位置。
     * 字符位置。实际内存位置为 基地址 + 字符位置 × 2。
     */
    size_t currentScreenPos;


protected:
    /**
     * 设置屏幕位置值。
     * 注意，该函数不会更新 cursorX 和 cursorY，需手动更新以确保值正确。
     * 
     * @param pos 
     */
    void setCurrentScreenPos(size_t pos);

};
