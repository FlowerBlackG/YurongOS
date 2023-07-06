// SPDX-License-Identifier: MulanPSL-2.0

/*
 * console
 *
 * 创建于 2023年7月6日 上海市嘉定区安亭镇
 */

#pragma once

#include <lib/sys/types.h>
#include <video/svga/svga.h>

namespace video {
namespace console {

/**
 * 初始化。
 * 依赖 svga。需要先初始化 svga，再调用本函数。
 */
void init();

int32_t write(const char* buf, int32_t len = -1, int32_t color = 0xffffff);

}
}
