;
; 内核加载器
; 创建于 2022年6月30日
;
; 大小：2KB
; 功能：
;   内存检测：获知内存大小，可使用区域等。
;
; --------------------------------
;

;
; 内存检测过程：
;
; BIOS 系统调用 15h 的 e820h 号子程序可以提供一定的信息（ARDS，内存范围描述结构）：
; 字节偏移  含义
; 0        基地址的低 32 位
; 4        基地址的高 32 位
; 8        内存长度的低 32 位（字节为单位）
; 12       内存长度的高 32 位
; 16       本段内存类型
; 
; 内存类型
; 1: 可用
; 其他: 不可用
;
; 调用输入：
;   eax 传入子程序号（e820h），ebx 传入0.
;   ebx 传入要读取的 ARDS 编号。第1次为0，之后该值会被设置。直接重复传入直到被设为0.
;   es:di 指向当前读到的 ARDS 缓冲区。
;   ecx: ARDS 结构字节大小。设为 20 字节。
;   edx: 固定为 0x534d4150 (SMAP 的 ASCII 码，是个校验签名)。
;
; 返回信息：
;   cf: 0 -> 正常，1 -> 出错
;   eax: 0x534d4150（SMAP 的 ASCII 码）
;   es:di, ecx: 与传入时一致。
;   ebx: 后续 ARDS 值。0表示检测结束。
;
; 参考文献：
;   踌躇月光.操作系统实现 - 008 内存检测.哔哩哔哩, 2022
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



; 内核加载器从 0x1000 位置开始。这个值可以随意，但需要保证选的内存位置可用。
org 0x1000

; 在开头设置魔数。加载后检查魔数是否正确。
; 若检测失败，表明加载器未被正确加载。
dw 0x55aa

mov si, msg_kernel_loading
call print

mov si, msg_detecting_memory
call print

; 检测内存。
detect_memory:
    ; 准备传入参数。
    xor ebx, ebx
    xor ax, ax
    mov es, ax
    mov di, ards_buffer
    mov edx, 0x534d4150

.detect_one:
    mov eax, 0xe820 ; 子功能号。
    mov ecx, 20
    int 0x15 ; 系统调用。

    ; 当 cf 置位（等于1），则跳转。
    jc error

    add edi, ecx ; 移动指针位置。
    inc word [ards_count] ; 计数。
    cmp ebx, 0
    jnz .detect_one

    ; 检测完毕。
    mov si, msg_detect_memory_success
    call print

    jmp prepare_protect_mode
    

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




msg_kernel_loading:
    db "info: loading kernel...", 0x0d, 0x0a, 0

msg_detecting_memory:
    db "info: detecing memory...", 0x0d, 0x0a, 0

msg_detect_memory_success:
    db "info: memory detection done.", 0x0d, 0x0a, 0

; 加载失败时，显示错误信息并停机。
error:
    mov si, .msg
    call print

.begin_hlt:
    hlt
    jmp .begin_hlt

.msg:
    db "error: exception occurred. failed to load kernel.", 0x0d, 0x0a, 0

; 准备保护模式。
prepare_protect_mode:

    cli ; 关中断。

    ; 加载 gdt。
    lgdt [gdt_pointer]

    ; 启动保护模式。
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; 用跳转来刷新缓存，以此启动保护模式。
    jmp dword code_selector:protect_mode



[bits 32] ; 提醒编译器，进入32位。

; 保护模式。同时，加载并进入内核。
protect_mode:
    mov ax, data_selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax ; 初始化段寄存器。

    mov esp, 0x10000 ; 修改栈顶。值随便，但需要是可用的。

    ; 读取 kernel 到内存。
    mov edi, 0x10000
    mov ecx, 10
    mov bl, 200
    call read_disk

    jmp dword code_selector:0x10000 ; 跳转到内核代码。

    jmp error ; 不可能执行到这里。


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


; 内存开始的位置（基地址）。
memory_base equ 0
; 内存界限（4G / 4K - 1）。
memory_limit equ ((1024 * 1024 * 1024 * 4) / (1024 * 4) - 1)

; 选择子。
code_selector equ (1 << 3)
data_selector equ (2 << 3)

gdt_pointer:
    dw (gdt_end - gdt_base) - 1 ; limit
    dd gdt_base ; base
gdt_base:
    dd 0, 0 ; dd = double words
; 代码段。
gdt_code:
    dw memory_limit & 0xffff ; limit low
    dw memory_base & 0xffff ; base low (0-15)
    db (memory_base >> 16) & 0xff ; base low (16-23)

    ; 存在，dlp 0，代码或数据，代码，非依从，可读，没被访问过。
    db 0b_1_00_1_1_0_1_0 
    ; 4K，32位，不是64位，available，limit high
    db 0b_1_1_0_0_0000 | (memory_limit >> 16) & 0xff
    db (memory_base >> 24) & 0xff ; base high
gdt_data:
    dw memory_limit & 0xffff ; limit low
    dw memory_base & 0xffff ; base low (0-15)
    db (memory_base >> 16) & 0xff ; base low (16-23)
 
    ; 存在，dlp 0，代码或数据，数据，向上扩展，可写，没被访问过。
    db 0b_1_00_1_0_0_1_0 
    ; 4K，32位，不是64位，available，limit high
    db 0b_1_1_0_0_0000 | (memory_limit >> 16) & 0xff
    db (memory_base >> 24) & 0xff ; base high
gdt_end:


ards_count:
    dw 0
ards_buffer:

