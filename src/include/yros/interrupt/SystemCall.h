/*

    系统调用

    创建于 2023年2月6日 江西省上饶市玉山县

*/

#pragma once

#include <yros/machine/X86Assembly.h>
#include <lib/sys/types.h>

struct Task;

namespace SystemCall {

    /**
     * 内核支持的系统调用最大数量。
     */
    const int SYSCALL_COUNT = 1024;

    /**
     * 指向系统调用处理函数的指针数组。
     */
    extern void* handlers[SYSCALL_COUNT];

    void init();

    void entrance();

    int64_t test();
    int64_t test2(int);
}
