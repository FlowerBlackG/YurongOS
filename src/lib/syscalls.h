/*

    系统调用 api

    创建于 2023年2月7日 江西省上饶市玉山县

*/

#pragma once

#include <lib/sys/types.h>

#ifdef __cplusplus

/**
 * 系统调用编号定义。
 */
enum class SystemCallId : int64_t {
    TEST_CALL = 0,
    WRITE = 1, 
    SLEEP = 2,
    NEW_PROCESS = 3
};

#endif


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


/**
 * 创建进程。
 * 调用号：3
 * 
 * @return int64_t 子进程id。
 */
int64_t newProcess();
