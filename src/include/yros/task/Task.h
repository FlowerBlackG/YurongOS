/*

    任务结构

    创建于2023年2月6日 江西省上饶市玉山县

*/

#pragma once

#include <lib/sys/types.h>

/**
 * 函数调用寄存器保存结构。
 * 遵循 System V ABI (amd64)
 */
struct TaskFrame {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t rbx;
    uint64_t rbp;
    uint64_t rip;
};


/**
 * 任务结构。
 */
struct Task {

    /**
     * 任务名称的最大长度。
     */
    static const int TASK_NAME_MAX_LENGTH = 16;


    /** 内核栈。 */
    unsigned long kernelStackPointer;

    /** 任务名称。 */
    char name[TASK_NAME_MAX_LENGTH];

    /**
     * 四级页表入口。
     */
    uint64_t pml4Address;

    /**
     * 进程id。
     */
    pid_t processId;

    /**
     * 母进程的id。
     */
    pid_t parentProcessId;


};
