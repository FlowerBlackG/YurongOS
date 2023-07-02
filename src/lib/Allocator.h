// SPDX-License-Identifier: MulanPSL-2.0

/*
 * 分配器。
 *
 * 创建于 2023年7月2日 上海市嘉定区安亭镇
 */

#pragma once

#include "./sys/types.h"

namespace lib {

struct Allocator {
    void* (* alloc) (size_t) = nullptr;
    void (* free) (void* addr) = nullptr;
};



}
