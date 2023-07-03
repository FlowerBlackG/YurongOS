// SPDX-License-Identifier: MulanPSL-2.0

/*

    任务结构

    创建于2023年2月6日 江西省上饶市玉山县

*/

#pragma once

#include <lib/sys/types.h>
#include <lib/collections/LinkedList.h>
#include <interrupt/SystemCall.h>

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


enum class TaskStatus {

    /**
     * 就绪。包含正在运行。
     */
    READY,

    RUNNING, // todo: 该状态未启用。

    /**
     * 阻塞。
     */
    BLOCKED,

    /**
     * 进程已死。准备被回收。
     */
    DIED
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


    SystemCall::SoftwareFrame* syscallSoftwareFrame;


    /** 任务名称。 */
    char name[TASK_NAME_MAX_LENGTH];

    /**
     * 四级页表入口。物理地址。
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

    bool kernelProcess;

    TaskStatus state;

    /**
     * 多功能链表节点。借助它，可以将进程“挂”到各种地方。
     * 主要用于将进程挂到某等待队列，如：
     *   · 等待 sleep 的唤醒
     *   · 等待读硬盘
     *   · 等待某进程挂掉
     */
    LinkedListNode linkedListNode;

    uint64_t wakeupMoment;


    /* ------ methods ------ */

    void putToSleep(uint64_t milliseconds);

};

#if 0
static void __task_check_size() {
    sizeof(Task);
};
#endif
