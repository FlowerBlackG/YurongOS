;
; TaskManager::switchTo(Task*)
; gcc 总做一些诡异的操作，因此使用纯汇编实现。
;
; 创建于2023年2月7日 江西省上饶市玉山县
;

[bits 64]

section .text

; TaskManager::switchTo(Task*)
global _ZN11TaskManager8switchToEP4Task
_ZN11TaskManager8switchToEP4Task:

    push rbp
    mov rbp, rsp

    push rbx
    push r12
    push r13
    push r14
    push r15

    mov rax, rsp
    and rax, 0xFFFFFFFFFFFFF000
    mov qword [rax], rsp

    mov qword rsp, [rdi]

    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp

    ret
