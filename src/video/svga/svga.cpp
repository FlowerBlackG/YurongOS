// SPDX-License-Identifier: MulanPSL-2.0

/*
 * SVGA
 *
 * 创建于 2023年7月5日 上海市嘉定区安亭镇
 */

#include "./svga.h"

#include <memory/memory.h>

namespace video {
namespace svga {


int8_t* screen;
int8_t bytesPerPixel;
VbeModeInfo* modeInfo;

void init(VbeModeInfo* modeInfo) {
    svga::modeInfo = modeInfo;
    svga::screen = (int8_t*) (modeInfo->framebuffer + memory::addrOfPhysicalMemoryMap);
    bytesPerPixel = modeInfo->bpp / 8;
}



}
}
