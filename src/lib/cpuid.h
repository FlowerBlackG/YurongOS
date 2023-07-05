// SPDX-License-Identifier: MulanPSL-2.0

/*
 * CPU ID Header
 *
 * 创建于 2023年7月3日 上海市嘉定区安亭镇
 */

#pragma once

#include <lib/sys/types.h>

#define __cpuid(level, a, b, c, d) \
    __asm ( \
        "cpuid \n\t" \
        : "=a" (a), "=b" (b), "=c" (c), "=d" (d) \
        : "0" (level) /* 为什么是 "0"? 我没看懂，但 gcc 和 llvm 都是这么干的。怪啊... */ \
    )

#define __cpuid(level, count, a, b, c, d) \
    __asm ( \
        "cpuid \n\t" \
        : "=a" (a), "=b" (b), "=c" (c), "=d" (d) \
        : "0" (level), "2" (count) \
    )


#define cpuid __cpuid

