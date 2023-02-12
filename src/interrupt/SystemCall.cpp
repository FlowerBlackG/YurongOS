/*

    系统调用

    创建于 2023年2月6日 江西省上饶市玉山县

*/

#include <yros/interrupt/SystemCall.h>
#include <yros/interrupt/ImplementHandlerMacros.h>

#include <yros/machine/X86Assembly.h>
#include <yros/machine/Msr.h>
#include <yros/machine/GlobalDescriptorTable.h>

#include <yros/memory/MemoryManager.h>
#include <yros/task/TaskManager.h>
#include <yros/PerCpuCargo.h>
#include <yros/machine/RflagsRegister.h>

#include <lib/sys/types.h>
#include <lib/stddef.h>
#include <lib/string.h>
#include <lib/stdio.h>

#include <yros/CRT.h>

namespace SystemCall {

#define SYSCALL_HANDLER(x) ((void*) x)

    void* handlers[SYSCALL_COUNT] = {
        SYSCALL_HANDLER(test),
        SYSCALL_HANDLER(test2)
    };

#undef SYSCALL_HANDLER

    void init() {
        Msr::write(Msr::SYSCALL_LSTAR, (uint64_t) SystemCall::entrance);
        
        Msr::write(
            Msr::SYSCALL_STAR, 
            (uint64_t(GlobalDescriptorTable::KERNEL_CODE_SELECTOR) << 32)
            | (uint64_t(GlobalDescriptorTable::USER_CODE_32_SELECTOR) << 48)
        );

        Msr::write(Msr::SYSCALL_SFMASK, RflagsRegister::IF_BIT);
    }

    auto pSc = &TaskManager::taskTable;

    void __omit_frame_pointer entrance() {

        x86asmSwapgs();
        
        __asm (
            "movq %%r12, %%gs:8 \n\t" // 暂存 r12
            "movq %%r13, %%gs:16 \n\t" // 暂存 r13
            
            "movq %%rsp, %%r12 \n\t"
            "movq %1, %%r13 \n\t"
            "cmpq %%r12, %%r13 \n\t"

            "jg _ZN10SystemCall8entranceEv.saveRegisters \n\t" // 跳转条件：已经在核心栈。

            "movq %%gs:0, %%r12 \n\t" // Cargo.self => r12
            "addq %0, %%r12 \n\t" // 令 r12 指向 currentTask 指针
            "movq (%%r12), %%r12 \n\t" // 核心栈 rsp => r12
            "xchgq %%r12, %%rsp \n\t" // 进入核心栈

        "_ZN10SystemCall8entranceEv.saveRegisters: \n\t"
            
            "pushq %%r12 \n\t" // old rsp
            "movq %%gs:8, %%r12 \n\t"
            "pushq %%r12 \n\t" // old r12
            "movq %%gs:16, %%r12 \n\t"
            "pushq %%r12 \n\t" // old r13
            "pushq %%r11 \n\t"
            "pushq %%rcx \n\t"
            "movq %%r10, %%rcx \n\t" 
            "pushq %%rbx \n\t"
            "movq %%ds, %%r12 \n\t"
            "pushq %%r12 \n\t"
            "movq %%es, %%r12 \n\t"
            "pushq %%r12 \n\t"
            
            :
            :
                "i" (offsetof(PerCpuCargo, currentTask)),
                "i" (MemoryManager::ADDRESS_OF_PHYSICAL_MEMORY_MAP)
        );

        x86asmLoadKernelDataSegments();
        
        __asm ("movq %rax, %rbx");
        
        __asm (
            "xchgq %%rax, %%rbx \n\t"
            "pushq %%rdx \n\t"
            "movq %0, %%r12 \n\t"
            "mulq %%r12 \n\t"
            "popq %%rdx \n\t"
            "addq %%rbx, %%rax \n\t"
            "movq (%%rax), %%rax \n\t"
            "call *%%rax \n\t"

            :
            : 
                "i" (sizeof(void*)),
                "a" (handlers)
        );

        __asm (
            "popq %%r12 \n\t"
            "movq %%r12, %%es \n\t"
            "popq %%r12 \n\t"
            "movq %%r12, %%ds \n\t"

            "popq %%rbx \n\t"
            "popq %%rcx \n\t"
            "popq %%r11 \n\t"
            "popq %%r13 \n\t"

            // 要恢复 task 结构内存储的栈顶指针。
            "movq %%gs:0, %%r12 \n\t"
            "addq %0, %%r12 \n\t"

            "addq $16, %%rsp \n\t"
            "movq %%rsp ,(%%r12) \n\t"
            "subq $16, %%rsp \n\t"

            "popq %%r12 \n\t"
            "popq %%rsp \n\t"
            
            :
            : 
                "i" (offsetof(PerCpuCargo, currentTask))
        );

        x86asmSwapgs();
        x86asmSti();
        x86asmSysretq();
    }

    void welcomeMat() {

    }

    int64_t test() {
        int x = 1;
        x++;
        x--;
        return 0;
    }

    int64_t test2(int x) {
     
        char s[32];
        sprintf(s, "number: %d\n", x);
        CRT::getInstance().write(s);
        return 0;
    }

}
