; 内核进入点。
; 创建于 2022年7月1日

[bits 32]

extern kernel_init

global _start
_start:
    call kernel_init
    jmp $
