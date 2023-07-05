// SPDX-License-Identifier: MulanPSL-2.0

/*

    任务管理器

    创建于2023年2月6日 江西省上饶市玉山县

*/

#include <task/TaskManager.h>
#include <misc/Kernel.h>
#include <machine/Machine.h>
#include <memory/KernelMemoryAllocator.h>
#include <memory/MemoryManager.h>
#include <machine/X86Assembly.h>
#include <machine/GlobalDescriptorTable.h>
#include <memory/PageDirectories.h>
#include <interrupt/InterruptExit.h>
#include <interrupt/SystemCall.h>
#include <interrupt/ClockInterrupt.h>
#include <misc/PerCpuCargo.h>

#include <lib/string.h>
#include <lib/stddef.h>

namespace TaskManager {

    Task* taskTable[TASK_TABLE_SIZE];

    void init() {
        memset(taskTable, 0, sizeof(taskTable));
    }

    Task* getCurrentTask() {

        intptr_t addr;


        __asm (
            "movq %%gs:0, %%r8 \n\t" // cargo.self => r8
            "addq %1, %%r8 \n\t" // ptr currentTask => r8
            "movq (%%r8), %%rax \n\t"

            :   "=a" (addr)
            : 
                "i" (offsetof(PerCpuCargo, currentTask))
            : 
                "%r8"
        );

        return (Task*) addr;
    }

    void schedule() {

        x86asmCli();

        static int prevId = 0;

        Task* next = nullptr;
        int prevIdBackup = prevId;
        
        do {
            prevId++;
            if (prevId >= TASK_TABLE_SIZE) {
                prevId = 0;
            }

            next = taskTable[prevId];

            if (next && next->status == TaskStatus::READY) {
                break;
            }

        } while (prevId != prevIdBackup);

        if (!next) {
            next = taskTable[0];
        }

        stage(next);
        x86asmSti();
        switchTo(next);

    } 

    void loadTaskToCargo(Task* task) {
        __asm (
            "movq %%gs:0, %%r8 \n\t" // cargo.self => r8
            "addq %0, %%r8 \n\t" // ptr currentTask => r8
            "movq %1, (%%r8) \n\t"

            :
            : 
                "i" (offsetof(PerCpuCargo, currentTask)),
                "r" (task)
            : 
                "%r8"
        );
    }

    void stage(Task* task) {
        auto& tss = GlobalDescriptorTable::taskStateSegment;
        unsigned long sp = (unsigned long) task->kernelStackPointer;
        sp &= 0xFFFFFFFFFFFFF000;
        sp += memory::MemoryManager::PAGE_SIZE;
        tss.rsp0Low = sp & 0xFFFFFFFF;
        tss.rsp0High = ((sp >> 16) >> 16) & 0xFFFFFFFF;

        loadTaskToCargo(task);

        Machine::setCR3(task->pml4Address);
    }


    Task* create(
        void (* target) (), 
        const char* name, 
        bool createTaskFrame,
        bool kernelProcess
    ) {

        bool prevInterruptFlag = Machine::getInstance().getAndSetInterruptState(false);

        pid_t pid = -1;

        for (pid_t i = 0; i < TASK_TABLE_SIZE; i++) {
            if (!taskTable[i]) {
                pid = i;
                break;
            }
        }

        if (pid < 0) {
            Kernel::panic("[error] task table fulled.\n");
        }

        if (pid && kernelProcess) {
            Kernel::panic("[error] kernel process can only be process 0.\n");
        }

        Task* task = (Task*) memory::KernelMemoryAllocator::allocPage();

        taskTable[pid] = task;
        task->processId = pid;
        task->kernelProcess = kernelProcess;
        strcpy(task->name, name);
        
        unsigned long kernelStackAddr;
        
        if (kernelProcess) {

            /* 
                系统启动过程中，以及 main 函数执行的时候，使用的栈和我们计划分配给
                kernel daemon 的是同一个。栈里还有少量数据。如果直接使用，会把少量
                但还需要使用的数据干碎，导致启动 kernel daemon 前系统就挂掉了。

                所以我们需要故意浪费一点空间。
            */

            const auto reserved = 1 * 1024 * 1024; // 1MB
            kernelStackAddr = memory::MemoryManager::KERNEL_PROCESS_STACK_BASE - reserved;

            * (long*) (memory::MemoryManager::KERNEL_PROCESS_STACK_TOP) = (long) task;
        } else {
            kernelStackAddr = (unsigned long) memory::KernelMemoryAllocator::allocPage();    
            * (long*) (kernelStackAddr) = (long) task;
            kernelStackAddr += memory::MemoryManager::PAGE_SIZE;
        }


        kernelStackAddr -= sizeof(InterruptHardwareFrame);
        auto hwContext = (InterruptHardwareFrame*) kernelStackAddr;
        kernelStackAddr -= sizeof(InterruptSoftwareFrame);
        auto swContext = (InterruptSoftwareFrame*) kernelStackAddr;

        int codeSelector;
        int dataSelector;
        if (kernelProcess) {
            codeSelector = GlobalDescriptorTable::KERNEL_CODE_SELECTOR;
            dataSelector = GlobalDescriptorTable::KERNEL_DATA_SELECTOR;
        } else {
            codeSelector = GlobalDescriptorTable::USER_CODE_64_SELECTOR;
            dataSelector = GlobalDescriptorTable::USER_DATA_SELECTOR;
        }

        swContext->ds = dataSelector;
        swContext->es = dataSelector;
        swContext->fs = 0;
        hwContext->ss = dataSelector;
        hwContext->cs = codeSelector;

        hwContext->rip = (unsigned long) target;
        hwContext->rflags = 0x0202; // reserved + interrupt enabled

        if (createTaskFrame) {
            kernelStackAddr -= sizeof(TaskFrame);
            TaskFrame* frame = (TaskFrame*) kernelStackAddr;
            frame->rip = (unsigned long) interruptExit;
        }

        task->kernelStackPointer = kernelStackAddr;

        if (kernelProcess) {
            task->pml4Address = memory::MemoryManager::KERNEL_PML4_ADDRESS;
            hwContext->rsp = memory::MemoryManager::KERNEL_PROCESS_STACK_BASE;

        } else {
            auto pml4 = (uint64_t) memory::KernelMemoryAllocator::allocWhitePage();

            uint64_t kernelPml4 = memory::MemoryManager::KERNEL_PML4_ADDRESS;
            kernelPml4 += memory::MemoryManager::ADDRESS_OF_PHYSICAL_MEMORY_MAP;

            memcpy((void*) pml4, (void*) kernelPml4, memory::MemoryManager::PAGE_SIZE);

            pml4 -= memory::MemoryManager::ADDRESS_OF_PHYSICAL_MEMORY_MAP;
            task->pml4Address = pml4;

            hwContext->rsp = memory::MemoryManager::USER_STACK_BASE;
        }

        Machine::getInstance().setInterruptState(prevInterruptFlag);
        return task;
    }

    void putToSleep(Task* task, uint64_t milliseconds) {
        ClockInterrupt::putToSleep(task, milliseconds);
    }
}
