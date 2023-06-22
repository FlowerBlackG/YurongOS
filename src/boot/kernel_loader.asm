; SPDX-License-Identifier: MulanPSL-2.0

;
;
;    内核加载器
;
;    被加载到内存 0x7800 的位置
;    最大尺寸为 1KB
;
;    本版创建于 2023年1月13日 安徽省安庆市桐城市
;    初版创建于 2022年6月30日 江西省上饶市玉山县
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


org 0x7800

dd 0x644f6f47 ; 魔数：GoOd

[bits 16] ; 提醒编译器后面是处于 16 位模式的代码。

; 四级页表位于 0x1000 处。
pml4_address equ 0x1000

jmp switch_to_long_mode

align 4 ; 4 字节对齐
empty_idt:
    .length dw 0
    .base dd 0


switch_to_long_mode:

    call check_cpu

    ; 先用 0 填充页表区域。
    
    mov edi, 0x1000
    mov ecx, (0x5000 / 4)
    call zero_fill_area

    mov edi, 0x9d000
    mov ecx, (0x2000 / 4)
    call zero_fill_area

    mov edi, 0x8000
    mov ecx, (64 * 0x1000 / 4)
    call zero_fill_area

.make_page_tables:

    ; 构建第4级页表（PML4）
    mov eax, 0x9d000
    or eax, 0x3 ; write, present
    mov [pml4_address], eax

    mov eax, 0x2000
    or eax, (0x3 | 0x4)  ; todo: 测试后删除 0x4
    mov [pml4_address + 320 * 8], eax

    mov eax, 0x4000
    or eax, 0x3
    mov [pml4_address + 383 * 8], eax

    mov eax, 0x8000
    or eax, 0x3
    mov edi, (pml4_address + 384 * 8)
    mov ecx, 384

.fill_pml4_physical_map_entries:

    mov [edi], eax

    add edi, 8
    add eax, 0x1000
    inc ecx
    cmp ecx, 448
    jne .fill_pml4_physical_map_entries

    ; 构建第3级页表（PML3）
    ; 不含物理映射区。
    mov eax, 0x9e000
    or eax, 0x3
    mov edi, 0x9d000
    mov [edi], eax

    mov eax, 0x3000
    or eax, (0x3 | 0x4)  ; todo: 测试后删除 0x4
    mov [0x2000], eax
    
    mov eax, 0x5000
    or eax, 0x3
    mov [0x4000 + 511 * 8], eax

    ; 构建第2级页表（PML2）
    ; 该表内的每条记录直接指向 2MB 内存。

    ; 前2M
    mov eax, 0x83 ; write, present, pageSize
    mov edi, 0x9e000
    mov [edi], eax

    ; 内核静态
    ; write, present, pageSize, global
    mov eax, (0x183 | 0x4)  ; todo: 测试后删除 0x4
    mov edi, 0x3000
    mov ebx, (0x3000 + 4 * 8)
    call build_pml2_linear_entries

    ; 核心栈
    mov eax, 0x80_0000
    or eax, 0x183
    mov edi, (0x5000 + 509 * 8)
    mov ebx, (0x5000 + 512 * 8)
    call build_pml2_linear_entries

    ; 页表准备就绪，可以进入内核。
    jmp enter_long_mode

; 构建 2MB 页映射。
; 参数：
;   edi: 页表项起始位置
;   ebx: 页表项最后一个位置的下一个位置
;   eax: 起始物理地址。应设置好属性。
build_pml2_linear_entries:

    mov [edi], eax

    add eax, 0x20_0000
    add edi, 8
    cmp edi, ebx
    jne build_pml2_linear_entries

    ret

enter_long_mode:

    lidt [empty_idt]

    ; 启用物理地址拓展和全局页表
    mov eax, cr4
    or eax, 0b10100000
    mov cr4, eax

    ; 设置 PML4 页表入口地址。
    mov edx, pml4_address
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
    

; 用 0 填充一个页框。
; 函数内不备份任何可能用到的寄存器。
;
; 传参：
;   edi: 起始地址。
;   ecx: 单元数。以 4 字节为 1 个单元。
;        必须大于 0。函数内不做正确性校验。
;
zero_fill_area:

    xor eax, eax

.zero_fill_dword:

    mov dword [edi], eax
    add edi, 4
    dec ecx

    cmp ecx, 0
    jne .zero_fill_dword

    ret


check_cpu:

    ;    原理见：
    ;        https://wiki.osdev.org/CPUID
    ;        https://en.wikipedia.org/wiki/CPUID 

    pushfd
    pop eax ; 得到 Flags
    mov ecx, eax
    xor eax, 0x200000 ; 如果不支持 CPUID 指令，这位在 Flags 寄存器里恒为0.
    push eax
    popfd ; 手动设置 Flags 寄存器。

    pushfd
    pop eax ; 重新得到 Flags。如果设置的位被清空了，表明 CPUID 指令不可用。
    xor eax, ecx
    shr eax, 21
    and eax, 1
    push ecx
    popfd

    test eax, eax
    jz .bad_cpu

    mov eax, 0x80000000 ; 获知可查询的功能范围。
    cpuid

    cmp eax, 0x80000001
    jb .bad_cpu

    mov eax, 0x80000001
    cpuid

    ; 检查 cpu 是否支持 long mode
    test edx, 1 << 29 
    jz .bad_cpu
    
    ; 检查 cpu 是否支持 1GB 页。
    test edx, 1 << 26
    jz .bad_cpu

    ret

.bad_cpu:
    jmp error


error:
    mov si, .msg
    call print

.begin_hlt:
    hlt
    jmp .begin_hlt

.msg:
    db "error: failed to boot kernel.", 0x0d, 0x0a, 0


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
    mov ss, ax

    mov ax, 0
    mov fs, ax
    mov gs, ax

    mov rsp, 0xFFFF_C000_0000_0000
    
    ; 构建物理映射区的 PML3。
    ; 这些表的每一条记录都指向 1GB 内存。

    mov rdi, 0x8000
    mov rcx, 0x48000
    mov rax, 0x183
.fill_pml3_physical_map_entries:

    mov qword [rdi], rax
    add rax, 0x4000_0000
    add rdi, 8
    cmp rdi, rcx
    jne .fill_pml3_physical_map_entries

    ; 刷新页表缓存。
    mov qword rax, pml4_address
    mov cr3, rax 

    ; 进入高位地址空间。
    mov rax, 0xFFFF_C000_0000_0000
    add rax, .enter_kernel
    jmp rax

.enter_kernel:

    ; 取消前 2M 直接映射。
    mov word [pml4_address], 0

    ; 刷新页表缓存。
    mov qword rax, pml4_address
    mov cr3, rax 

    ; 跳转进入内核代码。
    mov rax, 0xFFFF_A000_0010_0000
    jmp rax
