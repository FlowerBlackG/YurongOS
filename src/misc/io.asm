; SPDX-License-Identifier: MulanPSL-2.0



; io 操作。
; 创建于 2022年7月2日。

[bits 64]

section .text ; 代码段。

global __io_in_byte
__io_in_byte:
    ; 保存栈帧。
    push rbp
    mov rbp, rsp

    xor rax, rax
    mov rdx, rdi ; 端口号只能是立即数或放在 dx 寄存器。

    in al, dx ; 将端口号 dx 的 8 bit 输入到 al。接收到的数据只能存入 ax。

    leave ; 恢复栈帧。
    ret


global __io_in_word
__io_in_word:
    ; 保存栈帧。
    push rbp
    mov rbp, rsp

    xor rax, rax
    mov rdx, rdi ; 端口号只能是立即数或放在 dx 寄存器。

    in ax, dx ; 将端口号 dx 的 16 bit 输入到 ax。接收到的数据只能存入 ax。

    leave ; 恢复栈帧。
    ret

global __io_out_byte
__io_out_byte:
    push rbp
    mov rbp, rsp

    mov rdx, rdi ; port
    mov rax, rsi ; data
    out dx, al

    leave
    ret

global __io_out_word
__io_out_word:
    push rbp
    mov rbp, rsp

    mov rdx, rdi ; port
    mov rax, rsi ; data
    out dx, ax

    leave
    ret
