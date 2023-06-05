/*

    系统调用

    创建于 2023年2月6日 江西省上饶市玉山县

*/

#pragma once

#include <machine/X86Assembly.h>
#include <lib/sys/types.h>

struct Task;

namespace SystemCall {

    /**
     * 系统调用软件帧。
     */
    struct SoftwareFrame {
        uint64_t es;
        uint64_t ds;
        uint64_t rbx;
        uint64_t rcx;
        uint64_t r11;
        uint64_t r13;
        uint64_t r12;
        uint64_t rsp;
    };

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


    int64_t testCall();
    int64_t write(int64_t fd, const char* buffer, size_t count);
    int64_t sleep(int64_t milliseconds);
    int64_t fork();

}
