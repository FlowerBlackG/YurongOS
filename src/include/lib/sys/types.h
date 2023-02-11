/*
 * 数据类型定义。
 * 创建于 2022年7月2日。
 */

#pragma once

#include <lib/stdint.h>

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

#ifndef __no_return
    #define __no_return __attribute__((__noreturn__))
#endif

#ifndef __omit_frame_pointer
    #define __omit_frame_pointer __attribute__((optimize("omit-frame-pointer")))
#endif

#ifndef __force_inline
    #define __force_inline __attribute__((always_inline)) inline
#endif

/** 内联汇编。 */
#ifndef __asm
    #define __asm __asm__ __volatile__
#endif

#ifndef __section
    #define __section(name) __attribute__((section(name)))
#endif

typedef unsigned long size_t;

typedef int32_t pid_t;
