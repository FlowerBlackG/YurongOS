/*

    任务管理器

    创建于2023年2月6日 江西省上饶市玉山县

*/

#include <yros/task/TaskManager.h>
#include <yros/Kernel.h>
#include <yros/machine/Machine.h>
#include <yros/memory/KernelMemoryAllocator.h>
#include <yros/memory/MemoryManager.h>
#include <yros/machine/X86Assembly.h>
#include <yros/machine/GlobalDescriptorTable.h>
#include <yros/memory/PageDirectories.h>
#include <yros/interrupt/InterruptExit.h>
#include <yros/interrupt/SystemCall.h>
#include <yros/PerCpuCargo.h>

#include <lib/string.h>
#include <lib/stddef.h>

namespace TaskManager {

    Task* taskTable[TASK_TABLE_SIZE];

    void init() {
        memset(taskTable, 0, sizeof(taskTable));
    }

    Task* getCurrentTask() {

        unsigned long addr;

        __asm (
            "movq %%rsp, %%rax"
            : "=a" (addr)
            :
        );

        addr &= 0xFFFFFFFFFFFFF000;

        return * (Task**) addr;
    }

    void schedule() {

        x86asmCli();

        static int prevId = 0;

        Task* next = nullptr;
        
        while (true) {
            prevId++;
            if (prevId >= TASK_TABLE_SIZE) {
                prevId = 0;
            }

            next = taskTable[prevId];
            x86asmBochsMagicBreakpoint();

            if (next && next->state == TaskState::READY) {
                break;
            }
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
        sp += MemoryManager::PAGE_SIZE;
        tss.rsp0Low = sp & 0xFFFFFFFF;
        tss.rsp0High = ((sp >> 16) >> 16) & 0xFFFFFFFF;

        uint64_t rsp;
        __asm ("movq %%rsp, %0" :"=m"(rsp):);
        if (rsp < MemoryManager::ADDRESS_OF_PHYSICAL_MEMORY_MAP - 1024ULL*1024*1024) {
            int x=  1;
            x++;
            x--;
        }

        loadTaskToCargo(task);

        const auto kernelPml4Addr = MemoryManager::KERNEL_PML4_ADDRESS 
            + MemoryManager::ADDRESS_OF_PHYSICAL_MEMORY_MAP;
        
        auto kernelPml4 = (PageMapLevel4) kernelPml4Addr;

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

        Task* task = (Task*) KernelMemoryAllocator::allocPage();

        taskTable[pid] = task;
        task->processId = pid;
        task->kernelProcess = kernelProcess;
        strcpy(task->name, name);
        
        unsigned long kernelStackAddr;
        
        if (kernelProcess) {
            kernelStackAddr = MemoryManager::KERNEL_PROCESS_STACK_BASE - 1 * 1024 * 1024;
            * (long*) (MemoryManager::KERNEL_PROCESS_STACK_TOP) = (long) task;
        } else {
            kernelStackAddr = (unsigned long) KernelMemoryAllocator::allocPage();    
            * (long*) (kernelStackAddr) = (long) task;
            kernelStackAddr += MemoryManager::PAGE_SIZE;
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
            task->pml4Address = MemoryManager::KERNEL_PML4_ADDRESS;
            hwContext->rsp = MemoryManager::KERNEL_PROCESS_STACK_BASE;

        } else {
            auto pml4 = (uint64_t) KernelMemoryAllocator::allocWhitePage();

            uint64_t kernelPml4 = MemoryManager::KERNEL_PML4_ADDRESS;
            kernelPml4 += MemoryManager::ADDRESS_OF_PHYSICAL_MEMORY_MAP;

            memcpy((void*) pml4, (void*) kernelPml4, MemoryManager::PAGE_SIZE);

            pml4 -= MemoryManager::ADDRESS_OF_PHYSICAL_MEMORY_MAP;
            task->pml4Address = pml4;

            hwContext->rsp = MemoryManager::USER_STACK_BASE;
        }

        Machine::getInstance().setInterruptState(prevInterruptFlag);
        return task;
    }
}
