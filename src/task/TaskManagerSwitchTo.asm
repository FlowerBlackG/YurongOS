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

    mov rbx, 0xFFFFC00000000000
    cmp rax, rbx
    jg .save_rsp

    mov rax, 0xFFFFBFFFFFA00000

.save_rsp:

    and rax, 0xFFFFFFFFFFFFF000 ; 得到指向 Task 的指针。
    mov rax, [rax] ; 得到存储内核栈 rsp 的地址。
    
    mov [rax], rsp
    
    mov rsp, [rdi]

    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp

    ret
