; io 操作。
; 创建于 2022年7月2日。

[bits 32]

section .text ; 代码段。

global io_in_byte
io_in_byte:
    ; 保存栈帧。
    push ebp
    mov ebp, esp

    xor eax, eax





    mov edx, [ebp + 8] ; todo 为什么是+8？ ;;;;;;;;;;;;;;
    
    
    
    
    
    
    in al, dx ; 将端口号 dx 的 8 bit 输入到 ax。

    leave ; 恢复栈帧。
    ret


global io_in_word
io_in_word:
    ret

global io_out_byte
io_out_byte:
    ret

global io_out_word
io_out_word:
    ret
