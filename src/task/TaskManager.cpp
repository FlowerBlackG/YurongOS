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

#include <lib/string.h>

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

        addr &= 0xFFFFFFFFFFFFF000ULL;
        return (Task*) addr;
    }

    void schedule() {

        x86asmCli();

        static int prevId = 0;
        
        while (false) {
            prevId++;
            if (prevId >= TASK_TABLE_SIZE) {
                prevId = 0;
            }

            if (taskTable[prevId]) {
                break;
            }
        }

        Task* t = taskTable[prevId];
        stage(t);
        //x86asmSti();
        switchTo(t);

    }

    void stage(Task* task) {
        auto& tss = GlobalDescriptorTable::taskStateSegment;
        unsigned long sp = (unsigned long) task;
        sp += MemoryManager::PAGE_SIZE;
        tss.rsp0Low = sp & 0xFFFFFFFF;
        tss.rsp0High = ((sp >> 16) >> 16) & 0xFFFFFFFF;

        Machine::setCR3(task->pml4Address);
    }


    void __task_to_user_mode(Task* task, void (* target) ()) {


        PageMapLevel4 pml4 = (PageMapLevel4) KernelMemoryAllocator::allocPage();

        task->pml4Address = (uint64_t) pml4 - MemoryManager::ADDRESS_OF_PHYSICAL_MEMORY_MAP;
        
        memcpy(pml4, MemoryManager::getKernelPml4(), MemoryManager::PAGE_SIZE);

        PageMapLevel3 pml3 = (PageMapLevel3) KernelMemoryAllocator::allocPage();
        memset(pml3, 0, MemoryManager::PAGE_SIZE);
        pml4[255].pageFrameNumber = ((uint64_t) pml3 - MemoryManager::ADDRESS_OF_PHYSICAL_MEMORY_MAP) >> 12;
        pml4[255].us = 1;
        pml4[255].present = 1;
        pml4[255].rw = 1;

        PageMapLevel2 pml2 = (PageMapLevel2) KernelMemoryAllocator::allocPage();
        memset(pml2, 0, MemoryManager::PAGE_SIZE);
        pml3[511].pageFrameNumber = ((uint64_t) pml2 - MemoryManager::ADDRESS_OF_PHYSICAL_MEMORY_MAP) >> 12;
        pml3[511].us = 1;
        pml3[511].present = 1;
        pml3[511].rw = 1;

        PageMapLevel1 pml1 = (PageMapLevel1) KernelMemoryAllocator::allocPage();
        memset(pml1, 0, MemoryManager::PAGE_SIZE);
        pml2[511].pageFrameNumber = ((uint64_t) pml1 - MemoryManager::ADDRESS_OF_PHYSICAL_MEMORY_MAP) >> 12;
        pml2[511].us = 1;
        pml2[511].present = 1;
        pml2[511].rw = 1;

        uint64_t stackPage = (uint64_t) KernelMemoryAllocator::allocPage();
        pml1[511].us = 1;
        pml1[511].present = 1;
        pml1[511].rw = 1;
        pml1[511].pageFrameNumber = (stackPage - MemoryManager::ADDRESS_OF_PHYSICAL_MEMORY_MAP) >> 12;
        

        

        uint64_t stackAddr = stackPage;
        stackAddr += MemoryManager::PAGE_SIZE;
        

        uint64_t addr = (uint64_t) task + MemoryManager::PAGE_SIZE;
        addr -= sizeof(HardwareContextRegisters);
        HardwareContextRegisters* hw = (HardwareContextRegisters*) addr;
        addr -= sizeof(SoftwareContextRegisters);
        SoftwareContextRegisters* sw = (SoftwareContextRegisters*) addr;

        task->kernelStackPointer = addr;


        hw->rip = (uint64_t) target;
        hw->rflags = (0 << 12 | 0b10 | 1 << 9);
        hw->rsp = (uint64_t) task + MemoryManager::PAGE_SIZE;MemoryManager::USER_STACK_BASE;
        sw->rbp = addr + sizeof(SoftwareContextRegisters);
        sw->r12 = 0xfb;
        /*
        hw->cs = GlobalDescriptorTable::USER_CODE_SELECTOR;
        hw->ss = GlobalDescriptorTable::USER_DATA_SELECTOR;
        sw->ds = GlobalDescriptorTable::USER_DATA_SELECTOR;
        sw->es = GlobalDescriptorTable::USER_DATA_SELECTOR;
        sw->gs = GlobalDescriptorTable::USER_DATA_SELECTOR;
        sw->fs = GlobalDescriptorTable::USER_DATA_SELECTOR;*/


        hw->cs = GlobalDescriptorTable::KERNEL_CODE_SELECTOR;
        hw->ss = GlobalDescriptorTable::KERNEL_DATA_SELECTOR;
        sw->ds = GlobalDescriptorTable::KERNEL_DATA_SELECTOR;
        sw->es = GlobalDescriptorTable::KERNEL_DATA_SELECTOR;
        sw->gs = GlobalDescriptorTable::KERNEL_DATA_SELECTOR;
        sw->fs = GlobalDescriptorTable::KERNEL_DATA_SELECTOR;
    }

    Task* create(void (* target) (), const char* name, bool __2u) {

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

        Task* task = (Task*) KernelMemoryAllocator::allocPage();
        
        taskTable[pid] = task;
        task->processId = pid;
        strcpy(task->name, name);
        
        unsigned long kernelStackAddr = (unsigned long) task + MemoryManager::PAGE_SIZE;

        kernelStackAddr -= sizeof(TaskFrame);
        TaskFrame* frame = (TaskFrame*) kernelStackAddr;

        task->kernelStackPointer = kernelStackAddr;

        frame->rip = (unsigned long) target;

        if (__2u) {
            __task_to_user_mode(task, target);
        }

        Machine::getInstance().setInterruptState(prevInterruptFlag);

        return task;
    }



}
