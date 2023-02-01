;
; 启动引导程序（主引导扇区）
; 创建于：2023年1月5日
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
    mov esp, 0x7800

    ; 打开 A20 线。
    ; 若不打开，会触发回绕，即第20位（从0开始计数）及以上值被丢弃（取模）。
    in al, 0x92
    or al, 2
    out 0x92, al

    call detect_memory ; 执行内存检测。
    mov si, msg_memory_detect_done
    call print

    ; 进入 unreal mode (big real mode)
    push ds
    lgdt [unreal_mode_gdt_pointer]

    mov eax, cr0
    or al, 1
    mov cr0, eax
    jmp 0x8:.temporary_protected_mode

.temporary_protected_mode:
    mov bx, 0x10
    mov ds, bx

    and al, 0xfe
    mov cr0, eax
    jmp 0:.unreal_mode

.unreal_mode:
    pop ds


    ; 加载二级加载器。
    mov bl, 2
    mov ecx, 2
    mov edi, 0x7800 ; 将二级启动器加载到内存 0x7800 的位置。
    call read_disk

    cmp dword [0x7800], 0x644f6f47 ; 检查魔数：GoOd
    jnz error
    mov si, msg_second_loader_read
    call print

    ; 加载内核代码。要求用户内存不小于 10MB。
    ; 内核代码共 7MB，从 1MB 位置开始存放（0x 10 0000）
    ; 每次读 128个扇区，即 64KB。总共读 112 轮。
    mov ecx, 0
    mov edx, 0x100000

    mov si, msg_reading_kernel
    call print

.loop_read_kernel:
    push ecx
    push edx

    mov bl, 128
    mov edi, edx
    add ecx, 4
    call read_disk

    pop edx
    pop ecx
    inc ecx
    add edx, (512 * 128)
    cmp ecx, 16 ; 只读 16 轮。
    jne .loop_read_kernel 

    mov si, msg_kernel_read
    call print

    ; 跳转到二级启动器。
    jmp 0:0x7804 ; 前 4 字节是魔数。


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
;   ecx: ARDS 结构字节大小。
;        该结构本为 20 字节，拓展填充至 24 字节。
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


; 检测内存。
; 结果存放位置：
;   count: 0x500
;   buffer: 0x504
detect_memory:
    ; 准备传入参数。
    xor ebx, ebx
    xor eax, eax
    mov es, ax
    mov di, 0x508
    mov edx, 0x534d4150
    mov dword eax, [0x500]

.detect_one:
    mov eax, 0xe820 ; 子功能号。
    mov ecx, 20
    int 0x15 ; 系统调用。

    ; 当 cf 置位（等于1），则跳转。
    jc error

    cmp cl, 20
    jne error

    add edi, 24 ; 移动指针位置。
    inc word [0x500] ; 计数。
    cmp ebx, 0
    jnz .detect_one

    ; 检测完毕。

    ret

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
    db "error: failed to load kernel.", 0x0d, 0x0a, 0


msg_memory_detect_done:
    db "i: mem detect done.", 0x0d, 0x0a, 0
    
msg_second_loader_read:
    db "i: 2nd loader read.", 0x0d, 0x0a, 0

msg_reading_kernel:
    db "i: reading kernel...", 0x0d, 0x0a, 0

msg_kernel_read:
    db "i: kernel read.", 0x0d, 0x0a, 0

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


times 510 - ($ - $$) db 0 ; 填充

; 设置主引导扇区的最后2字节魔数内容。
db 0x55, 0xaa ; bios 要求的。否则会认为硬盘有问题。

