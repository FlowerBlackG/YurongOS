;
; 启动引导程序（主引导扇区）
; 创建于：2022年6月25日
;
; 尺寸：512 字节
; 功能：加载内核启动器并执行
; 结构：
;     代码：446 字节
;     硬盘分区表：64 字节（4 × 16 字节）
;     魔数：0xaa55
;
; 参考文献：
;   踌躇月光.操作系统实现 - 002 配置开发环境.bilibili, 2022
;   judyge.利用BIOS 中断INT 0x10显示字符和字符串.CSDN, 2016
;   Wikipedia.INT 10H.Wikipedia
;   
; --------------------------------
;

; org 含义是 origin，起始地址。
; 规定程序的起始地址从 0x7c00 开始。
; BIOS 完成自检后，会把主引导扇区读取到这个位置，并从这个位置开始执行代码。
; 进入 0x7c00 时，cpu 处于实模式，16位。
org 0x7c00

[bits 16]

; 设置屏幕模式为文本模式，并清空屏幕。
; 中断指令号为 10H，当 AH=0H 时表示设置显示模式，模式具体为 AL。
; AL=3H 表示字符串包含
mov ax, 3
int 0x10

; 初始化段寄存器。
xor ax, ax
mov ds, ax
mov es, ax
mov ss, ax
mov sp, 0x7c00

; 展示启动信息。
mov si, bootmsg
call print

; 打开 A20 线。
; 若不打开，会触发回绕，即第20位（从0开始计数）及以上值被丢弃（取模）。
in al, 0x92
or al, 0b10
out 0x92, al

; 加载 内核加载器 到内存。
mov edi, 0x1000
mov ecx, 2
mov bl, 4
call read_disk

; 检查是否读入正确。
cmp word [0x1000], 0x55aa
jnz error

; 跳转到 kernel loader 内的代码。
; 0x1000 开始的前2个字节为魔数。往后才是代码内容。
jmp 0:0x1002

; 读硬盘。
; 参数：edi 读取的目标内存。
;      ecx 起始扇区。
;      bl 扇区数。
read_disk:
    ; 设置读取扇区的数量。
    mov dx, 0x1f2 ; 0x1f2 是设置读写扇区数量的端口。
    mov al, bl
    out dx, al

    ; 设置读取扇区号。
    ; 0x1f3 低8位
    ; 0x1f4 中8位
    ; 0x1f5 高8位

    inc dx ; 0x1f3
    mov al, cl
    out dx, al

    inc dx ; 0x1f4
    shr ecx, 8
    mov al, cl
    out dx, al

    
    inc dx ; 0x1f5
    shr ecx, 8
    mov al, cl
    out dx, al

    inc dx ; 0x1f6
    shr ecx, 8
    and cl, 0x0f
    mov al, 0b1110_0000 ; 主盘，LBA 模式。
    or al, cl
    out dx, al

    inc dx ; 0x1f7
    mov al, 0x20 ; 0x20 表示读硬盘。
    out dx, al

    xor ecx, ecx
    mov cl, bl

.read:
    push cx
    call .wait
    call .read_sector
    pop cx
    loop .read

    ret

    .wait:
        mov dx, 0x1f7
        .check:
            in al, dx

            and al, 0b1000_1000
            cmp al, 0b0000_1000 ; 检查数据是否准备完毕。
            jnz .check

        ret
    
    .read_sector:
        mov dx, 0x1f0
        mov cx, 256 ; 一个扇区的大小是 256 个字。
        .read_word:
            in ax, dx

            mov [edi], ax
            add edi, 2

            loop .read_word
        ret


; 写硬盘。
; 参数：edi 读取的目标内存。
;      ecx 起始扇区。
;      bl 扇区数。
write_disk:
    ; 设置写入扇区的数量。
    mov dx, 0x1f2 ; 0x1f2 是设置读写扇区数量的端口。
    mov al, bl
    out dx, al

    ; 设置读取扇区号。
    ; 0x1f3 低8位
    ; 0x1f4 中8位
    ; 0x1f5 高8位

    inc dx ; 0x1f3
    mov al, cl
    out dx, al

    inc dx ; 0x1f4
    shr ecx, 8
    mov al, cl
    out dx, al

    
    inc dx ; 0x1f5
    shr ecx, 8
    mov al, cl
    out dx, al

    inc dx ; 0x1f6
    shr ecx, 8
    and cl, 0x0f
    mov al, 0b1110_0000 ; 主盘，LBA 模式。
    or al, cl
    out dx, al

    inc dx ; 0x1f7
    mov al, 0x30 ; 0x30 表示写硬盘。
    out dx, al

    xor ecx, ecx
    mov cl, bl

    .write:
        push cx
        call .write_sector
        call .wait
        pop cx
        loop .write

    ret

    .wait:
        mov dx, 0x1f7
        .check:
            in al, dx
            and al, 0b1000_0000
            cmp al, 0 ; 检查数据是否准备完毕。
            jnz .check

        ret
    
    .write_sector:
        mov dx, 0x1f0
        mov cx, 256 ; 一个扇区的大小是 256 个字。
        .write_word:
            mov ax, [edi]
            out dx, ax
            jmp $+2 ; 跳转到下一行。提供一些延迟。
            jmp $+2
            jmp $+2
            add edi, 2
            loop .write_word
        ret

; 输出字符串。
;
; 0xb8000 是文本显示器的内存区域。
; 实模式寻址时，有效地址 = 段地址 × 16 + 偏移地址。
; 有效地址（EA）= 0xb800 × 16 + 0 = 0xb8000
; 例如，我们可以这样输出内容：
;   mov ax, 0xb800
;   mov ds, ax
;   mov byte [0], 'q'
; 本 print 函数使用 int 10h 中断完成功能。
print:
    ; int 10h 的 0x0e 号子程序：Teletype output
    ; 传参：AL 传递字符，BL 传递颜色。
    mov ah, 0x0e
.then:
    mov al, [si]
    cmp al, 0
    jz .done
    int 0x10
    inc si
    jmp .then
.done:
    ret

bootmsg:
    db "info: booting YurongOS...", 0x0d, 0x0a, 0x0 ; \n\r

; 启动失败时，显示错误信息并停机。
error:
    mov si, .msg
    call print

.begin_hlt:
    hlt
    jmp .begin_hlt

.msg:
    db "error: failed to load kernel loader.", 0x0d, 0x0a, 0

times 510 - ($ - $$) db 0 ; 填充

; 设置主引导扇区的最后2字节魔数内容。
db 0x55, 0xaa ; bios 要求的。否则会认为硬盘有问题。
