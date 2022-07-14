/*
 * 可变参数。
 * 创建于 2022年7月13日。
 * 
 * 参考：
 *   https://pubs.opengroup.org/onlinepubs/9699919799/
 *   https://cplusplus.com/reference/cstdarg/
 */

#pragma once

#include <sys/types.h> // for NULL.

typedef uint32_t va_list;

#define va_start(ap, argN) (ap = (va_list) &argN + sizeof(argN))

#define va_copy(dest, src) (dest = src)

#define va_arg(ap, type) (*(type*)((ap += sizeof(type)) - sizeof(type)))

#define va_end(ap) (ap = (va_list) NULL)
