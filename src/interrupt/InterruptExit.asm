; 中断出口。
; 创建于 2023年2月8日 江西省上饶市玉山县。

[bits 64]

section .text ; 代码段。

global _Z13interruptExitv
_Z13interruptExitv:
    pop fs
    pop rax
    mov ds, rax
    pop rax
    mov es, rax
    pop rbx
    pop rcx
    pop rdx
    pop rsi
    pop rdi
    pop rbp
    pop rax
    pop r8
    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15

    add rsp, 8  ; 跳过 errcode

    cli
    cmp word [rsp + 0x8], 0x8
    je .end

    swapgs

.end:
    sti

    iretq
