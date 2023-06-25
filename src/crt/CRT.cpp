// SPDX-License-Identifier: MulanPSL-2.0

/*
 * CRT 实现。
 * 创建于 2022年7月6日。
 */

#include <crt/CRT.h>
#include <misc/io.h>
#include <lib/string.h>
#include <lib/AsciiChar.h>
#include <machine/Machine.h>

#include <misc/Kernel.h>
#include <concurrent/Mutex.h>

CRT CRT::instance;

static concurrent::Mutex crtMutex;

CRT::CRT() {
    
}

void CRT::init() {
    io::outByte(CRT::ADDR_REG, CRT::Reg::SCREEN_POS_HIGH);
    this->currentScreenPos = io::inByte(CRT::DATA_REG) << 8;
    io::outByte(CRT::ADDR_REG, CRT::Reg::SCREEN_POS_LOW);
    this->currentScreenPos |= io::inByte(CRT::DATA_REG);

    /* 获取光标位置。 */
    io::outByte(CRT::ADDR_REG, CRT::Reg::CURSOR_HIGH);
    uint16_t cursorPos = io::inByte(CRT::DATA_REG) << 8;
    io::outByte(CRT::ADDR_REG, CRT::Reg::CURSOR_LOW);
    cursorPos |= io::inByte(CRT::DATA_REG);

    cursorPos -= this->currentScreenPos;

    this->cursorX = cursorPos % CRT::COLS;
    this->cursorY = cursorPos / CRT::COLS;
}

void CRT::clear() {
    if (this->currentScreenPos * 2 + CRT::PAGE_MEMORY_SIZE * 2 <= CRT::MEMORY_SIZE) {
        this->setCurrentScreenPos(this->currentScreenPos + CRT::PAGE_MEMORY_SIZE / 2);
    } else {
        this->setCurrentScreenPos(0);
    }

    memset((void*) (CRT::MEMORY_BASE + this->currentScreenPos * 2), 0, CRT::PAGE_MEMORY_SIZE);
    moveCursor(0, 0);
}

void CRT::moveCursor(uint8_t x, uint8_t y) {
    this->cursorX = x < CRT::COLS ? x : CRT::COLS - 1;

    uint8_t yMax = CRT::PAGE_MEMORY_SIZE / 2 / CRT::COLS - 1;
    this->cursorY = y <= yMax ? y : yMax;

    uint16_t cursorPos = this->cursorY * CRT::COLS + this->cursorX;
    cursorPos += this->currentScreenPos;

    io::outByte(CRT::ADDR_REG, CRT::Reg::CURSOR_HIGH);
    io::outByte(CRT::DATA_REG, cursorPos >> 8);
    io::outByte(CRT::ADDR_REG, CRT::Reg::CURSOR_LOW);
    io::outByte(CRT::DATA_REG, cursorPos);
}

void CRT::setCharAttr(uint8_t attr) {
    this->charAttr = attr;
}

void CRT::setBackgroundColor(uint8_t color) {
    this->charAttr &= 0x8f; // 清空背景色，保留其他样式。
    this->charAttr |= (color & 0x7) << CharAttr::BACKGROUND_BITS_OFFSET;
}

void CRT::setForegroundColor(uint8_t color, uint8_t blink, uint8_t highlight) {

    crtMutex.lock();

    this->charAttr &= 0xf8; // 删除颜色属性。
    this->charAttr |= color & 0x7; // 设置新的颜色属性。

    // 设置闪烁属性。
    if (blink == 0) { // 否。
        this->charAttr &= ~CharAttr::BLINK_BIT;
    } else if (blink == 1) { // 是。
        this->charAttr |= CharAttr::BLINK_BIT;
    } else if (blink == 2) { // 翻转。
        this->charAttr ^= CharAttr::BLINK_BIT;
    }

    // 设置高亮属性。
    if (highlight == 0) { // 否。
        this->charAttr &= ~CharAttr::HIGH_LIGHT_BIT;
    } else if (highlight == 1) { // 是。
        this->charAttr |= CharAttr::HIGH_LIGHT_BIT;
    } else if (highlight == 2) { // 翻转。
        this->charAttr ^= CharAttr::HIGH_LIGHT_BIT;
    }

    crtMutex.unlock();
}

void CRT::scrollDown(uint16_t lines) {
    if (lines == 0) {
        return;
    }

    if (lines <= this->cursorY) {
        this->cursorY -= lines;
        size_t targetScreenPos = currentScreenPos + CRT::COLS * lines;
        if (targetScreenPos * 2 + CRT::PAGE_MEMORY_SIZE <= CRT::MEMORY_SIZE) {
            // 新的区域之前可能显示过其他内容。手动清空一下。


#if 1 // todo: 暂时很摆烂

            memset(
                (void*) (MEMORY_BASE + 2 * (targetScreenPos - lines * COLS) + PAGE_MEMORY_SIZE),
                0, 
                lines * 2 * CRT::COLS
            );
#endif

            this->setCurrentScreenPos(targetScreenPos);
        } else {
            // 将目标屏幕内容拷贝到 0 开始的位置。
            memcpy(
                (void*) CRT::MEMORY_BASE,
                (void*) (targetScreenPos * 2 + CRT::MEMORY_BASE),
                CRT::PAGE_MEMORY_SIZE - 2 * (targetScreenPos - currentScreenPos)
            );

            memset(
                (void*) (MEMORY_BASE - 2 * (lines * COLS) + PAGE_MEMORY_SIZE),
                0, 
                lines * 2 * CRT::COLS
            );

            this->setCurrentScreenPos(0);
            this->moveCursor(this->cursorX, this->cursorY);
        }
        
    } else {
        this->clear(); // 滚动行数超出屏幕高度了，直接清屏就行。
    }
}


void CRT::scrollUp(uint16_t lines) {

    if (lines == 0) {
        return;
    }

    // todo: 目前的实现非常摆烂。
    size_t targetScreenPos = currentScreenPos - CRT::COLS * lines;
    this->setCurrentScreenPos(targetScreenPos);
}


void CRT::setCurrentScreenPos(size_t pos) { // protected
    this->currentScreenPos = pos; // 内部调用，不做错误检测与处理。
    io::outByte(CRT::ADDR_REG, CRT::Reg::SCREEN_POS_HIGH);
    io::outByte(CRT::DATA_REG, this->currentScreenPos >> 8);
    io::outByte(CRT::ADDR_REG, CRT::Reg::SCREEN_POS_LOW);
    io::outByte(CRT::DATA_REG, this->currentScreenPos);
}

void CRT::putchar(uint8_t ch) {

    crtMutex.lock();

    // 光标所在位置的显示内存地址。
    char *cursorMemAddr = (char *) (
            MEMORY_BASE + (currentScreenPos + cursorY * COLS + cursorX) * 2
    );

    if (ch >= AsciiChar::START_OF_VISIBLE_CHARS && ch <= AsciiChar::END_OF_VISIBLE_CHARS) {
        // 输出字符。
        *cursorMemAddr = ch;
        *(cursorMemAddr + 1) = this->charAttr;

        // 移动光标。
        if (cursorX + 1 < CRT::COLS) {
            moveCursor(cursorX + 1, cursorY);
        } else {
            if (cursorY + 1 == PAGE_MEMORY_SIZE / 2 / COLS) {
                scrollDown(1);
            }
            moveCursor(0, cursorY + 1);
        }

    } else switch (ch) {
        case AsciiChar::CR: // \r
            moveCursor(0, cursorY);
            break;

        case AsciiChar::LF: // \n
            if (cursorY + 1 == PAGE_MEMORY_SIZE / 2 / COLS) {
                scrollDown(1);
            }
            moveCursor(cursorX, cursorY + 1);

            break;

        default:
            break;
    }

    crtMutex.unlock();

}


size_t CRT::write(const char* str, size_t len) {
    if (len == 0) {
        return 0;
    }

    const char* p = str;
    while (p - str < len && *p != AsciiChar::NUL) {

        // 对于 \n，如果前一个不是 \r，则补充输出 \r。
        if (*p == AsciiChar::LF && ((p > str && *(p - 1) != AsciiChar::CR) || p == str)) {
            CRT::putchar(AsciiChar::CR);
        }

        CRT::putchar(*p);
        ++p;
    }

    return p - str;
}
