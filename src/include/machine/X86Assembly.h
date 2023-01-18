/*

    created on 2022.11.28
*/


#pragma once

#include <sys/types.h>

#define x86asmSti() __asm ("sti")
#define x86asmCli() __asm ("cli")
#define x86asmLeave() __asm ("leave")
#define x86asmIret() __asm ("iretq")

#define x86asmDirectCall(function) __asm ("call *%%rax" :: "a" (function))

/**
 * 软件现场。
 */
struct SoftwareContextRegisters {
    uint64_t gs;
    uint64_t fs;
    uint64_t ds;
    uint64_t es;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t rax;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
};

/**
 * 硬件现场。
 */
struct HardwareContextRegisters {
    /**
     * 错误码。
     * 该错误码在 8 和 10-14 号中断到来时自动被设置。
     * 其他中断到来时，手动放入一个假的错误码。
     */
    uint64_t errorCode;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
};


/**
 * 保存软件现场。
 * 
 * 保存后，栈顶值指向一个 CommonRegisters 保存结构。
 * 栈顶下的第一个元素指向保存的硬件现场。
 */
#define x86asmSaveContext() \
    __asm ( \
        "cld \n\t" \
        "pushq %r15 \n\t" \
        "pushq %r14 \n\t" \
        "pushq %r13 \n\t" \
        "pushq %r12 \n\t" \
        "pushq %r11 \n\t" \
        "pushq %r10 \n\t" \
        "pushq %r9 \n\t" \
        "pushq %r8 \n\t" \
        "pushq %rax \n\t" \
        "pushq %rbp \n\t" \
        "pushq %rdi \n\t" \
        "pushq %rsi \n\t" \
        "pushq %rdx \n\t" \
        "pushq %rcx \n\t" \
        "pushq %rbx \n\t" \
        "mov %es, %rax \n\t" \
        "pushq %rax \n\t" \
        "mov %ds, %rax \n\t" \
        "pushq %rax \n\t" \
        "pushq %fs \n\t" \
        "pushq %gs \n\t" \
        "mov %rsp, %rdi \n\t" \
        "lea 0x8(%rbp), %rsi \n\t" \
    )

#define x86asmRestoreContext() \
    __asm ( \
        "popq %gs \n\t" \
        "popq %fs \n\t" \
        "popq %rax \n\t" \
        "mov %rax, %ds \n\t" \
        "popq %rax \n\t" \
        "mov %rax, %es \n\t" \
        "popq %rbx \n\t" \
        "popq %rcx \n\t" \
        "popq %rdx \n\t" \
        "popq %rsi \n\t" \
        "popq %rdi \n\t" \
        "popq %rbp \n\t" \
        "popq %rax \n\t" \
        "popq %r8 \n\t" \
        "popq %r9 \n\t" \
        "popq %r10 \n\t" \
        "popq %r11 \n\t" \
        "popq %r12 \n\t" \
        "popq %r13 \n\t" \
        "popq %r14 \n\t" \
        "popq %r15 \n\t" \
    )
