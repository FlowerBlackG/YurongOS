// SPDX-License-Identifier: MulanPSL-2.0


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
        int64_t rax;
        int64_t es;
        int64_t ds;
        int64_t rbx;
        int64_t rcx;
        int64_t r11;
        int64_t r13;
        int64_t r12;
        int64_t rsp;
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


    void testCall();
    void write(int64_t fd, const char* buffer, size_t count);
    void sleep(int64_t milliseconds);
    void newProcess();

}

#if 0

void __check_size() {
    sizeof(SystemCall::SoftwareFrame);
}
#endif
