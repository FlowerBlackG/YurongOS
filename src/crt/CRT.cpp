/*
 * CRT 实现。
 * 创建于 2022年7月6日。
 */

#include <CRT.h>
#include <yros/IO.h>
#include <string.h>

CRT::CRT() {
    /* 获取光标位置。 */
    IO::outByte(CRT::ADDR_REG, CRT::Reg::CURSOR_HIGH);
    uint16_t cursorPos = IO::inByte(CRT::DATA_REG) << 8;
    IO::outByte(CRT::ADDR_REG, CRT::Reg::CURSOR_LOW);
    cursorPos |= IO::inByte(CRT::DATA_REG);
    this->cursorX = cursorPos % CRT::COLS;
    this->cursorY = cursorPos / CRT::COLS;
}

void CRT::clear() {
    if (this->currentMemoryStart + CRT::PAGE_MEMORY_SIZE * 2 <= CRT::MEMORY_SIZE) {
        this->currentMemoryStart += CRT::PAGE_MEMORY_SIZE;
    } else {
        memset((void*) CRT::MEMORY_BASE, 0, CRT::PAGE_MEMORY_SIZE);
        this->currentMemoryStart = CRT::MEMORY_BASE;
    }

    IO::outByte(CRT::ADDR_REG, CRT::Reg::CURSOR_HIGH);
    IO::outByte(CRT::DATA_REG, this->currentMemoryStart << 8);
    IO::outByte(CRT::ADDR_REG, CRT::Reg::CURSOR_LOW);
    IO::outByte(CRT::DATA_REG, this->currentMemoryStart);
}

