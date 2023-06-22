; SPDX-License-Identifier: MulanPSL-2.0

; 内核进入点。
; 创建于 2022年7月1日

[bits 64]

extern kernel_bridge

global kernel_entry
kernel_entry:

    mov qword rax, kernel_bridge
    call rax

.beg_hlt:
    hlt
    jmp .beg_hlt
