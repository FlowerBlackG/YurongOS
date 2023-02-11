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

#include <lib/sys/types.h>

namespace SystemCall {

    struct {

        int paramCount;
        int64_t (* handler) ();
    
    } handlers[SYSCALL_NUM] = {
        
        { 0, test },
        { 0, test }
    
    };

    void init() {
        Msr::write(Msr::SYSCALL_LSTAR, (uint64_t) SystemCall::entrance);
        
        Msr::write(
            Msr::SYSCALL_STAR, 
            (uint64_t(GlobalDescriptorTable::KERNEL_CODE_SELECTOR) << 32)
            | (uint64_t(GlobalDescriptorTable::USER_CODE_32_SELECTOR) << 48)
        );
    }

    auto pSc = &TaskManager::taskTable;

    void __omit_frame_pointer entrance() {

        x86asmCli();
        x86asmSwapgs();

        __asm (

            "movq %%rsp, %%r12 \n\t"
            "movq %0, %%r13 \n\t"
            "cmpq %%r12, %%r13 \n\t"
            "jg _ZN10SystemCall8entranceEv.saveRegisters \n\t" // 跳转条件：已经在核心栈。



            "_ZN10SystemCall8entranceEv.saveRegisters: \n\t"
             

            "pushq %%rcx \n\t"
            "movq %%r10, %%rcx \n\t" 

            "pushq %%r12 \n\t" // old rbp
            "pushq %%r13 \n\t" // old rsp
            "pushq %%rbx \n\t"

            "movq %%ds, %%r12 \n\t"
            "pushq %%r12 \n\t"
            "movq %%es, %%r12 \n\t"
            "pushq %%r12 \n\t"
            "pushq %%fs \n\t"
            "pushq %%gs \n\t"

// todo

            :

            :
                "i" (MemoryManager::ADDRESS_OF_TASK_POINTERS),
                "i" (MemoryManager::ADDRESS_OF_PHYSICAL_MEMORY_MAP)
        );

        x86asmLoadKernelDataSegments();

        __asm (
            ""

            :

            :
        );


        x86asmSwapgs();
        x86asmSti();

    }

    void welcomeMat() {

    }

    int64_t test() {
        return 0;
    }

}
