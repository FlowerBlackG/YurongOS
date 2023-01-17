

; created on 2022.11.29

[bits 64]

extern interruptHandlers

section .text

%macro INTERRUPT_HANDLER_BRIDGE 2
interrupt_handler_bridge_%1:

%ifn %2
    push 0x20222202
%endif
    push %1
    jmp interrupt_entry

%endmacro

interrupt_entry:
    push ds
    push es
    push fs
    push gs
    pushad ; push all general regs, 共 8 个

    mov eax, [esp + 12 * 4] ; 找到中断向量。

    push eax
    call [interruptHandlers + eax * 4]

global interrupt_exit
interrupt_exit:
    
    add esp, 4

    popad
    pop gs
    pop fs
    pop es
    pop ds

    add esp, 8
    iret


INTERRUPT_HANDLER_BRIDGE 0x00, 0
INTERRUPT_HANDLER_BRIDGE 0x01, 0
INTERRUPT_HANDLER_BRIDGE 0x02, 0
INTERRUPT_HANDLER_BRIDGE 0x03, 0

INTERRUPT_HANDLER_BRIDGE 0x04, 0
INTERRUPT_HANDLER_BRIDGE 0x05, 0
INTERRUPT_HANDLER_BRIDGE 0x06, 0
INTERRUPT_HANDLER_BRIDGE 0x07, 0

INTERRUPT_HANDLER_BRIDGE 0x08, 1
INTERRUPT_HANDLER_BRIDGE 0x09, 0
INTERRUPT_HANDLER_BRIDGE 0x0a, 1
INTERRUPT_HANDLER_BRIDGE 0x0b, 1

INTERRUPT_HANDLER_BRIDGE 0x0c, 1
INTERRUPT_HANDLER_BRIDGE 0x0d, 1
INTERRUPT_HANDLER_BRIDGE 0x0e, 1
INTERRUPT_HANDLER_BRIDGE 0x0f, 0

INTERRUPT_HANDLER_BRIDGE 0x10, 0
INTERRUPT_HANDLER_BRIDGE 0x11, 1
INTERRUPT_HANDLER_BRIDGE 0x12, 0
INTERRUPT_HANDLER_BRIDGE 0x13, 0

INTERRUPT_HANDLER_BRIDGE 0x14, 0
INTERRUPT_HANDLER_BRIDGE 0x15, 1
INTERRUPT_HANDLER_BRIDGE 0x16, 0
INTERRUPT_HANDLER_BRIDGE 0x17, 0

INTERRUPT_HANDLER_BRIDGE 0x18, 0
INTERRUPT_HANDLER_BRIDGE 0x19, 0
INTERRUPT_HANDLER_BRIDGE 0x1a, 0
INTERRUPT_HANDLER_BRIDGE 0x1b, 0

INTERRUPT_HANDLER_BRIDGE 0x1c, 0
INTERRUPT_HANDLER_BRIDGE 0x1d, 0
INTERRUPT_HANDLER_BRIDGE 0x1e, 0
INTERRUPT_HANDLER_BRIDGE 0x1f, 0

INTERRUPT_HANDLER_BRIDGE 0x20, 0
INTERRUPT_HANDLER_BRIDGE 0x21, 0
INTERRUPT_HANDLER_BRIDGE 0x22, 0
INTERRUPT_HANDLER_BRIDGE 0x23, 0
INTERRUPT_HANDLER_BRIDGE 0x24, 0
INTERRUPT_HANDLER_BRIDGE 0x25, 0
INTERRUPT_HANDLER_BRIDGE 0x26, 0
INTERRUPT_HANDLER_BRIDGE 0x27, 0
INTERRUPT_HANDLER_BRIDGE 0x28, 0
INTERRUPT_HANDLER_BRIDGE 0x29, 0
INTERRUPT_HANDLER_BRIDGE 0x2a, 0
INTERRUPT_HANDLER_BRIDGE 0x2b, 0
INTERRUPT_HANDLER_BRIDGE 0x2c, 0
INTERRUPT_HANDLER_BRIDGE 0x2d, 0
INTERRUPT_HANDLER_BRIDGE 0x2e, 0
INTERRUPT_HANDLER_BRIDGE 0x2f, 0


section .data
global interrupt_handler_bridges
interrupt_handler_bridges:
    dd interrupt_handler_bridge_0x00
    dd interrupt_handler_bridge_0x01
    dd interrupt_handler_bridge_0x02
    dd interrupt_handler_bridge_0x03
    dd interrupt_handler_bridge_0x04
    dd interrupt_handler_bridge_0x05
    dd interrupt_handler_bridge_0x06
    dd interrupt_handler_bridge_0x07
    dd interrupt_handler_bridge_0x08
    dd interrupt_handler_bridge_0x09
    dd interrupt_handler_bridge_0x0a
    dd interrupt_handler_bridge_0x0b
    dd interrupt_handler_bridge_0x0c
    dd interrupt_handler_bridge_0x0d
    dd interrupt_handler_bridge_0x0e
    dd interrupt_handler_bridge_0x0f
    dd interrupt_handler_bridge_0x10
    dd interrupt_handler_bridge_0x11
    dd interrupt_handler_bridge_0x12
    dd interrupt_handler_bridge_0x13
    dd interrupt_handler_bridge_0x14
    dd interrupt_handler_bridge_0x15
    dd interrupt_handler_bridge_0x16
    dd interrupt_handler_bridge_0x17
    dd interrupt_handler_bridge_0x18
    dd interrupt_handler_bridge_0x19
    dd interrupt_handler_bridge_0x1a
    dd interrupt_handler_bridge_0x1b
    dd interrupt_handler_bridge_0x1c
    dd interrupt_handler_bridge_0x1d
    dd interrupt_handler_bridge_0x1e
    dd interrupt_handler_bridge_0x1f
    dd interrupt_handler_bridge_0x20
    dd interrupt_handler_bridge_0x21
    dd interrupt_handler_bridge_0x22
    dd interrupt_handler_bridge_0x23
    dd interrupt_handler_bridge_0x24
    dd interrupt_handler_bridge_0x25
    dd interrupt_handler_bridge_0x26
    dd interrupt_handler_bridge_0x27
    dd interrupt_handler_bridge_0x28
    dd interrupt_handler_bridge_0x29
    dd interrupt_handler_bridge_0x2a
    dd interrupt_handler_bridge_0x2b
    dd interrupt_handler_bridge_0x2c
    dd interrupt_handler_bridge_0x2d
    dd interrupt_handler_bridge_0x2e
    dd interrupt_handler_bridge_0x2f

section .text
