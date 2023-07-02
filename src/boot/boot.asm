; SPDX-License-Identifier: MulanPSL-2.0

;
; 启动引导程序（主引导扇区）
; 
; 本版创建于 2023年1月5日 北京市海淀区
; 初版创建于 2022年6月25日 江西省上饶市玉山县
;
; 尺寸：512 字节
;
; 参考文献：
;   踌躇月光.操作系统实现 - 002 配置开发环境.bilibili, 2022
;   judyge.利用BIOS 中断INT 0x10显示字符和字符串.CSDN, 2016
;   Wikipedia.INT 10H.Wikipedia
;   同济大学 Unix V6++ OOS
;   
; --------------------------------
;


; 
; 实模式内存布局
;
; 起始地址    大小       用途
; ----------------------------------------
; 0x000      1KB       中断向量表
; 0x400      256B      BIOS 数据区
; 0x500      29.75KB   可用区域
; 0x7C00     512B      MBR 加载区域
; 0x7E00     607.6KB   可用区域
; 0x9FC00    1KB       扩展 BIOS 数据区
; 0xA0000    64KB      用于彩色显示适配器
; 0xB0000    32KB      用于黑白显示适配器
; 0xB8000    32KB      用于文本显示适配器
; 0XC0000    32KB      显示适配器 BIOS
; 0XC8000    160KB     映射内存
; 0xF0000    64KB-16B  系统BIOS
; 0xFFFF0    16B       系统 BIOS 入口地址
; 


;
; 实模式与保护模式
;
; 刚开机时，CPU运行于实模式，只能访问 1M内存，且其他程序可以完全控制计算机。
; 
; 保护模式：
;   寄存器：有一些只能被系统访问
;   高速缓存
;   内存：使用描述符保护内存
;   外部设备，如硬盘。in / out 指令需要操作系统使用。
; 
; 
; 全局描述符（8字节）
;   内存的起始位置
;   内存的长度 / 界限
;   内存属性
;   
;   名称          长度（bit）    含义
;   ------------------------------------------------
;   limit (low)    16        段界限 第0-15位
;   base (low)     24        基地址第0-23位。可表示 16M
;   type            4        段类型
;   segment         1        1 代码段或数据段，0 系统段
;   DPL             2        描述符特权等级 0-3. 0最高，操作系统。3应用程序。
;   present         1        存在位。1在内存，0在磁盘
;   limit (high)    4        段界限 第16-19位
;   available       1
;   long mode       1        64位扩展标志
;   big             1        32位还是64位
;   granularity     1        粒度是4KB或1B
;   base (high)     8        基地址第24-31位
;
;  type: X | C/E | R/W | A （从高到低）
;    x: 1代码，0数据
;    A: 是否被CPU访问过
;    
;            X=1              x=0
;    --------------------------------------
;    C/E   是否是依从代码段  0向上扩展/1向下扩展
;    R/W   是否可读         是否可写
;  
;    依从代码段：应用程序可以直接跳到的位置。否则不让跳（如操作系统的代码段）。
; 
; 全局描述符表 GDT Global Descriptor Table
;   顺序表、链表、哈希表...
;   descriptor gdt[8192]
;   英特尔要求：gdt[0] 为全0
;   最多有8191个可用描述符。
; 
; 描述符指针：limit (2字节), base (4字节)
; gdtr 寄存器：全局描述符表的起始位置和长度
; 指令：lgdt [指针]：加载 gdt
;      sgdt [指针]：保存 gdt
; 
; 段选择子
;   只需要一个代码段
;   需要一个或多个数据段 / 栈段
;   加载到段寄存器中 / 校验特权级
;
;   名称    长度（bit）    含义
;   ---------------------------------------
;   RPL      2          Request PL
;   TI       1      0全局描述符，1局部描述符
;   index    13         全局描述符表索引
;   
;   相关寄存器：cs 代码段 / ds 数据段 / es 扩展段 / ss 栈段 / fs / gs
;
; 启动保护模式（PE, Protect Enable）
;   将 cr0 寄存器 0位 置1
;


; org 含义是 origin，起始地址。
; 规定程序的起始地址从 0x7c00 开始。
; BIOS 完成自检后，会把主引导扇区读取到这个位置，并从这个位置开始执行代码。
; 进入 0x7c00 时，cpu 处于实模式，16位。
org 0x7c00

[bits 16] ; 提醒编译器后面是处于 16 位模式的代码。

start:

    ; 关中断。
    cli
    mov al, 0xff
    out 0xa1, al
    out 0x21, al

    jmp 0:.flush_cs

.flush_cs:

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
    mov esp, 0x7800 ; 暂用的栈。

    ; 打开 A20 线。
    ; 若不打开，会触发回绕，即第20位（从0开始计数）及以上值被丢弃（取模）。
    in al, 0x92
    or al, 2
    out 0x92, al

    ; 进入 unreal mode (big real mode)
    ; 这样才能访问 1M 以上内存。

    push ds
    lgdt [unreal_mode_gdt_pointer]

    mov eax, cr0
    or al, 1
    mov cr0, eax
    jmp 0x8:.temporary_protected_mode ; 先进一下保护模式

.temporary_protected_mode:
    mov bx, 0x10
    mov ds, bx

    and al, 0xfe
    mov cr0, eax
    jmp 0:.unreal_mode ; 欸嘿，离开保护模式

.unreal_mode:

    pop ds

    ; 加载二级加载器。
    mov ebx, 2
    mov ecx, 2
    mov edi, 0x7800 ; 将二级启动器加载到内存 0x7800 的位置。
    call read_disk

    cmp dword [0x7800], 0x644f6f47 ; 检查魔数：GoOd
    jnz error
    mov si, msg_second_loader_read
    call print

    ; 加载内核代码。要求用户内存不小于 10MB。
    ; 内核代码共 7MB，从 1MB 位置开始存放（0x 10 0000）
    ; 每“轮”读 128个扇区，即 64KB。总共读 112 轮。
    
    mov si, msg_reading_kernel
    call print

    mov ebx, 4 ; 读取目标扇区号。从第4个扇区开始读。
    mov ecx, (128 * 112) 
    mov edi, 0x100000 ; 存放内存位置。

    call read_disk

    mov si, msg_kernel_read
    call print

    ; 跳转到二级启动器。
    jmp 0:0x7804 ; 前 4 字节是魔数。


; 输出字符串。
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


; 加载失败时，显示错误信息并停机。
error:
    mov si, .msg
    call print

.begin_hlt:
    hlt
    jmp .begin_hlt

.msg:
    db "failed !", 0x0d, 0x0a, 0


msg_second_loader_read:
    db "2 ldr ok", 0x0d, 0x0a, 0

msg_reading_kernel:
    db "rd krnl..", 0x0d, 0x0a, 0

msg_kernel_read:
    db "krnl rd", 0x0d, 0x0a, 0

; 读硬盘。
; 考虑到安装的设备可能很复杂，
; 这里直接使用 bios 13h 号程序。
;
; 模式：该函数只能在 real mode 下被调用。
;       unreal mode 也可以。其内部使用了传统段机制。
;
; 参数：edi 读取的目标内存。
;      ebx 起始扇区。
;      ecx 扇区数。
read_disk:
    ; 实现思路：
    ;     由于 int 13h 难以访问 1MB 以上内存，
    ;     我们先将数据读取到 1MB 以下的一个缓存，
    ;     再手动拷贝到 1MB 以上空间。
    ;
    ;     这样会带来一些时间开销，但 io 显然更慢。
    ;

    ; size of packet (1 Byte)
    mov byte [disk_addr_pack_size_of_dap], 16

    ; zero (1 Byte)
    mov byte [disk_addr_pack_zero], 0

    ; buffer - offset (2 Bytes)
    mov eax, disk_buffer_addr
    and eax, 0xFFFF  ; 16 bits
    mov word [disk_addr_pack_buf_off], ax

    ; buffer - segment (2 Bytes)
    mov eax, disk_buffer_addr
    and eax, 0xFFFF_0000  ; upper 2 Bytes
    shr eax, 4
    mov word [disk_addr_pack_buf_seg], ax

.read_loop:

    ; num of sectors (2 Bytes)
    ; eax 表示本次读取多少个扇区。
    mov eax, ecx
    cmp eax, disk_buffer_size_sects
    jle .set_num_of_sects 
    mov eax, disk_buffer_size_sects

.set_num_of_sects:

    sub ecx, eax  ; ecx 存储剩余扇区数。

    mov word [disk_addr_pack_num_of_sect], ax

    ; lower 32 bits of LBA (4 Bytes)
    mov dword [disk_addr_pack_lba_low], ebx
    add ebx, eax

    ; upper 16 bits of LBA (4 Bytes)
    mov word [disk_addr_pack_lba_high], 0

    push eax
    push edi
    push ecx
    push ebx

    ; 读取硬盘
    mov si, disk_addr_packet
    mov ah, 0x42  ; 读取硬盘
    mov dl, 0x80  ; 目标：0 号硬盘
 
    int 0x13
    jc .error

    pop ebx
    pop ecx
    pop edi
    pop eax

    ; 内存拷贝。
    push ecx
    push ebx

    shl eax, 9 ; 每扇区 512 字节。
    mov ecx, eax ; 字节数
    mov ebx, edi ; 目标地址
    add edi, eax
    mov eax, disk_buffer_addr ; 源地址
    call memcpy

    pop ebx
    pop ecx

   

    cmp ecx, 0
    jnz .read_loop

    ret

.error:
    jmp error


disk_buffer_addr equ 0x58000
disk_buffer_end equ  0x60000
disk_buffer_size equ (disk_buffer_end - disk_buffer_addr) ; 要求是 2 的 X 次方（X 至少是 9）。
disk_buffer_size_sects equ (disk_buffer_size / 512)
disk_buffer_size_sects_mask equ (disk_buffer_size_sects - 1)


; 地址信息包（disk address packet, DAP）
; https://wiki.osdev.org/Disk_access_using_the_BIOS_(INT_13h)
; 要求按照 4 字节对齐。
;
; offset size desc
; -------------------------------------
; 0      1  size of DAP (just set it to 16 bytes)
; 1      1  always 0
; 2      2  number of sectors to transfer      
; 4      2  transfer buffer (offset)
; 6      2  transfer buffer (segment)
; 8      4  lower 32 bits of 48-bit starting LBA 
; 12     4  upper 16 bits of 48-bit starting LBA
;
disk_addr_packet equ 0xFF0
disk_addr_pack_size_of_dap equ (disk_addr_packet + 0)
disk_addr_pack_zero equ (disk_addr_packet + 1)
disk_addr_pack_num_of_sect equ (disk_addr_packet + 2)
disk_addr_pack_buf_off equ (disk_addr_packet + 4)
disk_addr_pack_buf_seg equ (disk_addr_packet + 6)
disk_addr_pack_lba_low equ (disk_addr_packet + 8)
disk_addr_pack_lba_high equ (disk_addr_packet + 12)


; 内存拷贝
; 
; 参数：
;       eax: 源地址。建议 4 字节对齐。
;       ebx: 目标地址。建议 4 字节对齐。
;       ecx: 字节数。要求是 4 的倍数。
;
; 寄存器破坏：
;     edx, ecx, eax, ebx
memcpy:
.cp_loop:
    cmp ecx, 0
    jz .end
    mov dword edx, [eax]
    mov dword [ebx], edx
    sub ecx, 4
    add eax, 4
    add ebx, 4
    jmp .cp_loop
.end:
    ret


unreal_mode_gdt_pointer:
    dw unreal_mode_gdt_end - unreal_mode_gdt_zero - 1
    dd unreal_mode_gdt_zero
unreal_mode_gdt_zero:
    dq 0x0000000000000000
unreal_mode_gdt_code:
    dq 0x00009a000000ffff
unreal_mode_gdt_data:
    dq 0x00cf92000000ffff
unreal_mode_gdt_end:

db 0xfb  ; 用一个特殊的字节标记结尾。

times 510 - ($ - $$) db 0 ; 填充

; 设置主引导扇区的最后2字节魔数内容。
db 0x55, 0xaa ; bios 要求的。否则会认为硬盘有问题。

