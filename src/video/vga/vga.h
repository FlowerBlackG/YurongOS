// SPDX-License-Identifier: MulanPSL-2.0

/*
 * VGA
 * 
 * * deprecated! use svga instead. *
 *
 * 创建于 2023年7月4日 上海市嘉定区安亭镇
 */

#pragma once

#include <lib/sys/types.h>

namespace video {
namespace vga {

extern int8_t* screen;
extern int32_t width;
extern int32_t height;

void init(int8_t* screen, int32_t width, int32_t height);

void putPixel(int32_t x, int32_t y, int8_t color);

}
}
