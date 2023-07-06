// SPDX-License-Identifier: MulanPSL-2.0

/*
 * console
 *
 * 创建于 2023年7月6日 上海市嘉定区安亭镇
 */

#include "./console.h"
#include "./fonts.h"
#include "../svga/svga.h"
#include <concurrent/Mutex.h>
#include <lib/AsciiChar.h>
#include <lib/string.h>

namespace video {
namespace console {

static int32_t currentX = 0;
static int32_t currentY = 0;
static int32_t xLimit = 0;
static int32_t yLimit = 0;

static concurrent::Mutex writeMutex;

void init() {
    auto vbeInfo = svga::modeInfo;
    xLimit = vbeInfo->width / 8;
    yLimit = vbeInfo->height / 16;
}

static inline void scrollDown(int32_t lines) {
    if (lines <= 0) {
        return;
    }

    auto vbeInfo = svga::modeInfo;

    if (lines >= yLimit) {
        memset(
            svga::screen,
            0,
            vbeInfo->height * vbeInfo->pitch
        );
        currentY = 0;

    } else {

        memcpy(
            svga::screen,
            (void*) (intptr_t(svga::screen) + vbeInfo->pitch * lines * 16),
            vbeInfo->height * vbeInfo->pitch - vbeInfo->pitch * lines * 16
        );

        memset(
            (void*) (intptr_t(svga::screen) + vbeInfo->height * vbeInfo->pitch - vbeInfo->pitch * lines * 16),
            0,
            vbeInfo->pitch * lines * 16
        );
        currentY -= lines;

    }
}

static inline void putchar(uint8_t ch, int32_t color) {

    if (ch >= AsciiChar::START_OF_VISIBLE_CHARS && ch <= AsciiChar::END_OF_VISIBLE_CHARS) {

        int32_t idx = ch - ' ';
        for (int8_t row = 0; row < 16; row++) {
            auto byte = fonts[idx * 16 + row];
            for (int8_t col = 0; col < 8; col++) {

                svga::putPixel(
                    currentX * 8 + col,
                    currentY * 16 + row,
                    (byte & (1 << (8 - col)) ? color : 0)
                );

            }
        }

        currentX++;
        if (currentX >= xLimit) {
            currentX = 0;
            currentY++;
        }

        if (currentY >= yLimit) {
            scrollDown(1);
        }

    } else switch (ch) {
        case AsciiChar::CR: // \r
            currentX = 0;
            break;

        case AsciiChar::LF: // \n
            if (currentY + 1 == yLimit) {
                scrollDown(1);
            }

            currentY++;

            break;

        default:
            break;
    }

}

int32_t write(const char* buf, int32_t len, int32_t color) {

    writeMutex.lock();

    const char* p = buf;
    while ((len < 0 || p - buf < len) && *p != AsciiChar::NUL) {

        // 对于 \n，如果前一个不是 \r，则补充输出 \r。
        if (*p == AsciiChar::LF && ((p > buf && *(p - 1) != AsciiChar::CR) || p == buf)) {
            putchar(AsciiChar::CR, color);
        }

        putchar(*p, color);
        ++p;

    }

    writeMutex.unlock();

    return p - buf;
}

}
}