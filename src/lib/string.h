// SPDX-License-Identifier: MulanPSL-2.0

/*
 * 字符串处理函数头文件。
 * 创建于 2022年7月4日。
 * 
 * 参考：
 *   cppreference.com. Null-terminated byte strings
 *     https://en.cppreference.com/w/c/string/byte
 */

#pragma once

#include <lib/sys/types.h>

char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t count);
char* strcat(char* dest, const char* src);
char* strncat(char* dest, const char* src, size_t count);
size_t strlen(const char* str);
int strcmp(const char* lhs, const char* rhs);
int strncmp(const char* lhs, const char* rhs, size_t count);
char* strchr(const char* str, int ch);
char* strrchr(const char* str, int ch);
size_t strspn(const char* dest, const char* src);
size_t strcspn(const char* dest, const char* src);
char* strpbrk(const char* dest, const char* breakset);
char* strstr(const char* str, const char* substr);
char* strtok(char* str, const char* delim);

void* memchr(const void* ptr, int ch, size_t count);
int memcmp(const void* lhs, const void* rhs, size_t count);
void* memset(void* dest, int ch, size_t count);
void* memcpy(void* dest, const void* src, size_t count);

