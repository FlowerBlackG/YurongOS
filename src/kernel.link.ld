/* 内核构建链接脚本。 */

ENTRY(kernel_entry)

SECTIONS
{
    
    .text 0xffffa00000100000:
    {
        
        *(.text)
        *(.rodata*)
        . = ALIGN(4096);
    }
    
    .data :
    {
        
        *(.data)
        . = ALIGN(4096);

        /* 构造函数列表。
           参考：https://ftp.gnu.org/old-gnu/Manuals/ld-2.9.1/text/ld.txt */
        __CTOR_LIST__ = .;
        *(.init_array)
        __CTOR_END__ = .;

        . = ALIGN(4096);

        /* 析构函数列表。 */
        __DTOR_LIST__ = .;
        *(.dtors)
        __DTOR_END__ = .;

        . = ALIGN(4096);
    }


    .bss : 
    {
        __BSS_START__ = .;
     
        *(.bss)
        . = ALIGN(4096);

        __BSS_END__ = .;
    }

    .per_cpu_cargo :
    {
        __PER_CPU_CARGO_START__ = .;

        *(.per_cpu_cargo)
        . = ALIGN(4096);

        __PER_CPU_CARGO_END__ = .;
    }

}
