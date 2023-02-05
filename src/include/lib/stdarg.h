/*
 * 可变参数。
 * 创建于 2022年7月13日。
 * 
 * 参考：
 *   https://pubs.opengroup.org/onlinepubs/9699919799/
 *   https://cplusplus.com/reference/cstdarg/
 */

#pragma once

#include <lib/sys/types.h> // for NULL.

#if (__x86_64__)

// 参考 linux5.19 include/linux/stdarg.h
// 原来 gcc 编译期已经实现了这个啊... 害得我思考半天怎么写...

typedef __builtin_va_list va_list;

#define va_start(ap, argN) __builtin_va_start(ap, argN)

#define va_copy(dest, src) __builtin_va_copy(dest, src)

#define va_arg(ap, type) __builtin_va_arg(ap, type)

#define va_end(ap) __builtin_va_end(ap)

#elif (__i386__)

typedef uint32_t va_list;

#define va_start(ap, argN) (ap = (va_list) &argN + sizeof(argN))

#define va_copy(dest, src) (dest = src)

#define va_arg(ap, type) (*(type*)((ap += sizeof(type)) - sizeof(type)))

#define va_end(ap) (ap = (va_list) NULL)

#else

#error cpu arch not supported.

#endif
