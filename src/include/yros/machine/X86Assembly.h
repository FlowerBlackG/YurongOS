/*

    created on 2022.11.28
*/


#pragma once

#include <sys/types.h>

#define x86asmSti() __asm ("sti")
#define x86asmCli() __asm ("cli")
#define x86asmLeave() __asm ("leave")
#define x86asmIret() __asm ("iret")

#define x86asmDirectCall(function) __asm ("call *%%eax" :: "a" (function))

/**
 * 软件现场。
 */
struct CommonRegisters {
    uint32_t gs;
    uint32_t fs;
    uint32_t ds;
    uint32_t es;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;
    uint32_t eax;
};

/**
 * 硬件现场。
 */
struct ContextRegisters {
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;
};

/**
 * 保存软件现场。
 * 
 * 保存后，栈顶值指向一个 CommonRegisters 保存结构。
 * 栈顶下的第一个元素指向保存的硬件现场。
 */
#define x86asmSaveContext() \
    __asm ( \
        "pushl %eax \n\t" \
        "pushl %ebp \n\t" \
        "pushl %edi \n\t" \
        "pushl %esi \n\t" \
        "pushl %edx \n\t" \
        "pushl %ecx \n\t" \
        "pushl %ebx \n\t" \
        "pushl %es \n\t" \
        "pushl %ds \n\t" \
        "pushl %fs \n\t" \
        "pushl %gs \n\t" \
        "lea 0x4(%ebp), %edx \n\t" \
        "pushl %edx \n\t" \
        "lea 0x4(%esp), %edx \n\t" \
        "pushl %edx \n\t" \
    )

#define x86asmRestoreContext() \
    __asm ( \
        "addl $0x8, %esp \n\t" \
        "popl %gs \n\t" \
        "popl %fs \n\t" \
        "popl %ds \n\t" \
        "popl %es \n\t" \
        "popl %ebx \n\t" \
        "popl %ecx \n\t" \
        "popl %edx \n\t" \
        "popl %esi \n\t" \
        "popl %edi \n\t" \
        "popl %ebp \n\t" \
        "popl %eax \n\t" \
    )
