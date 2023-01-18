;
;
;    内核加载器
;
;    被加载到内存 0x7000 的位置
;    最大尺寸为 1KB
;
;    创建于 2023年1月13日
;
;



;
; 四级页表结构
;   x64 架构使用四级页表映射内存。
;   此时，分段功能被关闭（强制使用平铺模型）
;
;   虚拟内存寻址空间为 48 位，总共 256TB
;   通过四级页表，分别管理：
;     [0, 12): 物理地址。每页框 4KB。
;     [12, 21): page table entry
;     [21, 30): page middle directory
;     [30, 39): page upper directory
;     [39, 48): page global directory
;
;   每单张表都需要 4KB
;


org 0x7000

dd 0x644f6f47 ; 魔数：GoOd

[bits 16] ; 提醒编译器后面是处于 16 位模式的代码。

jmp switch_to_long_mode

align 4 ; 4 字节对齐
empty_idt:
    .length dw 0
    .base dd 0

switch_to_long_mode:
    ; 从 0x8000 开始，放置一个暂用的四级页表。
    ; [0x8000, 0x15000)，共 52KB。

    ; 0x8000 值存储到 di 寄存器内。

    ; 先用 0 填充。
    mov edi, 0x8000
    push edi

    mov ecx, ((0x15000 - 0x8000) / 4)
    xor eax, eax

.zero_out_paging_area_loop:

    mov dword [edi], eax
    add edi, 4

    dec ecx
    cmp ecx, 0
    jne .zero_out_paging_area_loop

    pop edi
    push edi

    ; 构建全局页表（第四级）。
    mov eax, 0x9000
    or eax, 0x3 ; write, present
    mov [0x8000], eax

    mov eax, 0xc000
    or eax, 0x3
    mov [0x8000 + 511 * 8], eax
    
    mov eax, 0x10000
    or eax, 0x3
    mov [0x8000 + 256 * 8], eax

    ; 构建第三级页表。
    mov eax, 0xa000
    or eax, 0x3
    mov [0x9000], eax

    mov eax, 0xd000
    or eax, 0x3
    mov [0xc000 + 511 * 8], eax

    mov eax, 0x11000
    or eax, 0x3
    mov edi, 0x10000
    mov dword [edi], eax

    ; 构建第二级页表。
    mov eax, 0xb000
    or eax, 0x3
    mov [0xa000], eax

    mov eax, 0xe000
    or eax, 0x3
    mov [0xd000 + 510 * 8], eax

    mov eax, 0xf000
    or eax, 0x3
    mov [0xd000 + 511 * 8], eax


    mov eax, 0x12000
    or eax, 0x3
    mov edi, 0x11000
    mov [edi], eax

    mov eax, 0x13000
    or eax, 0x3
    mov edi, (0x11000 + 8)
    mov [edi], eax

    mov eax, 0x14000
    or eax, 0x3
    mov edi, (0x11000 + 2 * 8)
    mov [edi], eax


    ; 构建一级页表。

    ;   0 ~ 1M
    mov ecx, 0
    mov edi, 0xb000
    mov ebx, 256
    mov eax, 0
    call .build_linear_page_tab_entry

    ; 核心栈。
    mov ecx, 256
    mov eax, 0x100000
    mov edi, (0xe000 + 8 * 256)
    mov ebx, 512
    call .build_linear_page_tab_entry
    
    mov ecx, 0
    mov eax, 0x200000
    mov edi, (0xf000)
    mov ebx, 512
    call .build_linear_page_tab_entry

    ; 内核。
    mov ecx, 0
    mov eax, 0x400000
    mov edi, (0x12000)
    mov ebx, 512
    call .build_linear_page_tab_entry
    
    mov ecx, 0
    mov eax, 0x600000
    mov edi, (0x13000)
    mov ebx, 512
    call .build_linear_page_tab_entry
    
    mov ecx, 0
    mov eax, 0x800000
    mov edi, (0x14000)
    mov ebx, 512
    call .build_linear_page_tab_entry

    jmp .build_paging_end

; edi: 页表所在位置
; ebx: 终止条目号
; ecx: 起始条目号
; eax: 起始物理地址
.build_linear_page_tab_entry:
    or eax, 0x3
    mov [edi], eax
    inc ecx
    add eax, 0x1000
    add edi, 8
    cmp ecx, ebx
    jne .build_linear_page_tab_entry
    ret

.build_paging_end:

    pop edi

enter_long_mode:

    lidt [empty_idt]

    ; 启用物理地址拓展和全局页表
    mov eax, cr4
    or eax, 0b10100000
    mov cr4, eax

    ; 设置 PML4 页表入口地址。
    mov edx, 0x8000
    mov cr3, edx

    ; 开启 long mode enable 和 syscall
    mov ecx, 0xc0000080
    rdmsr
    or eax, 0x0101 
    wrmsr

    lgdt [gdt_pointer]

    ; 同时启用 paging 和 保护模式
    mov ebx, cr0
    or ebx, 0x80000001

    mov cr0, ebx


    jmp code_selector:kernel_loader
    

; 选择子。
code_selector equ (1 << 3)
data_selector equ (2 << 3)

align 4

gdt_pointer:
    dw (gdt_end - gdt_base) - 1 ; limit
    
    dd gdt_base ; base
    dd 0

align 4

gdt_base:
    dq 0

; 代码段。
gdt_code:

    ; (idx)    : 1
    ; limit    : 0
    ; base     : 0
    ; access   : 0
    ; rw       : 1
    ; dc       : 0
    ; exec     : 1
    ; descType : code/data
    ; privi lv : 0
    ; present  : 1
    ; longMode : 1
    ; sizeFlag : 16 bits
    ; granular : 1 B

    dq 0x00209a0000000000
    
gdt_data:

    ; (idx)    : 2
    ; limit    : 0
    ; base     : 0
    ; access   : 0
    ; rw       : 1
    ; dc       : 0
    ; exec     : 0
    ; descType : code/data
    ; privi lv : 0
    ; present  : 1
    ; longMode : 0
    ; sizeFlag : 16 bits
    ; granular : 1 B

    dq 0x0000920000000000
    

gdt_end:


[bits 64] ; 提醒编译器后面是处于 64 位模式的代码。

kernel_loader:

    ; 初始化段寄存器。
    mov ax, data_selector

    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax 

    mov rsp, 0
    

    mov rax, 0xFFFF_8000_0000_0000
    jmp rax
