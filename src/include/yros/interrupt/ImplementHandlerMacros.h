/*

    中断处理通用入口和简单处理器。

    创建于2023年2月1日 江西省上饶市玉山县

*/

#pragma once

#include <yros/machine/X86Assembly.h>
#include <yros/interrupt/InterruptExit.h>
#include <yros/machine/GlobalDescriptorTable.h>
#include <yros/Kernel.h>

/*

    曾经的写法。为防止新的出错，旧的先留着：todo

    #define IMPLEMENT_EXCEPTION_ENTRANCE(entranceFunctionName, handlerName) \
    void __omit_frame_pointer entranceFunctionName()  { \
        __asm ("pushq $0x656e6f6e");  \
        x86asmSaveContext(); \
        \
        x86asmDirectCall(handlerName); \
        x86asmRestoreContext(); \
        __asm("addq $8, %rsp"); \
        x86asmIret(); \
    }

#define IMPLEMENT_EXCEPTION_WITH_ERRCODE_ENTRANCE(entranceFunctionName, handlerName) \
    void __omit_frame_pointer entranceFunctionName()  { \
        x86asmSaveContext(); \
        \
        x86asmDirectCall(handlerName); \
        x86asmRestoreContext(); \
        __asm ("addq $8, %rsp"); \
        x86asmIret(); \
    }


*/

#define __implHandlerMacros_x86asmSwapgsIfNecessary(functionName) \
    __asm ( \
        "cli \n\t" \
        "cmpw %0, 0x10(%%rsp) \n\t" \
        "je ." #functionName ".end \n\t" \
        "swapgs \n\t" \
        "." #functionName ".end: \n\t" \
        "sti \n\t" \
        \
        : \
        : "i" (GlobalDescriptorTable::KERNEL_CODE_SELECTOR) \
    )

#define IMPLEMENT_EXCEPTION_ENTRANCE(entranceFunctionName, handlerName) \
    void __omit_frame_pointer entranceFunctionName()  { \
        __asm ("pushq $0x656e6f6e"); /* ascii: none */ \
        __implHandlerMacros_x86asmSwapgsIfNecessary(entranceFunctionName); \
        x86asmSaveContext(); \
        x86asmLoadKernelDataSegments(); \
        \
        x86asmDirectCall(handlerName); \
        \
        x86asmNearJmp(interruptExit); \
    }

#define IMPLEMENT_EXCEPTION_WITH_ERRCODE_ENTRANCE(entranceFunctionName, handlerName) \
    void __omit_frame_pointer entranceFunctionName()  { \
        __implHandlerMacros_x86asmSwapgsIfNecessary(entranceFunctionName); \
        x86asmSaveContext(); \
        x86asmLoadKernelDataSegments(); \
        \
        x86asmDirectCall(handlerName); \
        \
        x86asmNearJmp(interruptExit); \
    }

#define IMPLEMENT_EXCEPTION_HANDLER(handlerName, errorMsg, signalValue) \
    void handlerName( \
        InterruptSoftwareFrame* softwareRegs, \
        InterruptHardwareFrame* hardwareRegs \
    ) { \
        x86asmCli(); \
        char s[256]; \
        \
        CRT::getInstance().write(" ------------\n  "); \
        CRT::getInstance().write(errorMsg); \
        CRT::getInstance().write("\n"); \
        \
        uint64_t cr2, cr3; \
        __asm( \
            "movq %%cr2, %%rax \n\t" \
            "movq %%cr3, %%rbx \n\t" \
            : "=a" (cr2), "=b" (cr3) \
            : \
        ); \
        \
        sprintf( \
            s, \
            "    rax: 0x%llx\n" \
            "    rbp: 0x%llx, rsp: 0x%llx\n" \
            "    cs: 0x%x, ds: 0x%x, es: 0x%x\n" \
            "    ss: 0x%x, fs: 0x%x, gs: 0x%x\n" \
            "    cr2: 0x%llx, cr3: 0x%llx\n" \
            "    rip: 0x%llx\n", \
            softwareRegs->rax, \
            softwareRegs->rbp, \
            hardwareRegs->rsp, \
            hardwareRegs->cs, softwareRegs->ds, softwareRegs->es, \
            hardwareRegs->ss, softwareRegs->fs, softwareRegs->gs, \
            cr2, cr3, \
            hardwareRegs->rip \
        ); \
        \
        CRT::getInstance().write(s); \
        CRT::getInstance().write(" ------------\n"); \
        \
        Kernel::panic("[critical] kernel panic.\n"); \
    }

#define IMPLEMENT_EXCEPTION_WITH_ERRCODE_HANDLER(handlerName, errorMsg, signalValue) \
    void handlerName( \
        InterruptSoftwareFrame* softwareRegs, \
        InterruptHardwareFrame* hardwareRegs \
    ) { \
        x86asmCli(); \
        char s[256]; \
        \
        CRT::getInstance().write(" ------------\n  "); \
        CRT::getInstance().write(errorMsg); \
        CRT::getInstance().write(" (with errcode)\n"); \
        \
        uint64_t cr2, cr3; \
        __asm( \
            "movq %%cr2, %%rax \n\t" \
            "movq %%cr3, %%rbx \n\t" \
            : "=a" (cr2), "=b" (cr3) \
            : \
        ); \
        \
        sprintf( \
            s, \
            "    errcode: 0x%llx\n" \
            "    rax: 0x%llx\n" \
            "    rbp: 0x%llx, rsp: 0x%llx\n" \
            "    cs: 0x%x, ds: 0x%x, es: 0x%x\n" \
            "    ss: 0x%x, fs: 0x%x, gs: 0x%x\n" \
            "    cr2: 0x%llx, cr3: 0x%llx\n" \
            "    rip: 0x%llx\n", \
            hardwareRegs->errorCode, \
            softwareRegs->rax, \
            softwareRegs->rbp, \
            hardwareRegs->rsp, \
            hardwareRegs->cs, softwareRegs->ds, softwareRegs->es, \
            hardwareRegs->ss, softwareRegs->fs, softwareRegs->gs, \
            cr2, cr3, \
            hardwareRegs->rip \
        ); \
        \
        CRT::getInstance().write(s); \
        CRT::getInstance().write(" ------------\n"); \
        \
        Kernel::panic("[critical] kernel panic.\n"); \
    }

#undef __x86asmSwapgsIfNecessary
