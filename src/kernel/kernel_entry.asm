; 内核进入点。
; 创建于 2022年7月1日

[bits 32]

extern call_kernel_modules_constructors
extern kernel_bridge

global kernel_entry
kernel_entry:
    call call_kernel_modules_constructors
    call kernel_bridge
.beg_hlt:
    hlt
    jmp .beg_hlt
