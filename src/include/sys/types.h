/*
 * 数据类型定义。
 * 创建于 2022年7月2日。
 */

#pragma once

#include <stdint.h>

/** 文件结尾。 */
#ifndef EOF
    #define EOF -1
#endif

/** 空指针。 */
#ifndef NULL
    #define NULL 0
#endif

/** 结构体紧凑。 */
#ifndef __packed
    #define __packed __attribute__((packed))
#endif

/** 内联汇编。 */
#ifndef __asm
    #define __asm __asm__ __volatile__
#endif

typedef unsigned long size_t;
