/*

    系统调用 api

    创建于 2023年2月7日 江西省上饶市玉山县

*/

#pragma once

#include <lib/SystemCallId.h>


/**
 * 
 * 调用号：0 
 */
int64_t testCall();

/**
 * 
 * 调用号：1 
 */
int64_t write(int64_t fd, const char* buffer, size_t count);

/**
 * 
 * 调用号：2 
 */
int64_t sleep(int64_t milliseconds);
