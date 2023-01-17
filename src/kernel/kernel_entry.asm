; 内核进入点。
; 创建于 2022年7月1日

[bits 64]

extern kernel_bridge

global kernel_entry
kernel_entry:
    call kernel_bridge
.beg_hlt:
    hlt
    jmp .beg_hlt
