/* 内核构建链接脚本。 */

ENTRY(kernel_entry)

SECTIONS
{
    .text 0x30000:
    {
        *(.text)
        . = ALIGN(4096);
    }
    
    .data :
    {
        /* 构造函数列表。
           参考：https://ftp.gnu.org/old-gnu/Manuals/ld-2.9.1/text/ld.txt */
        __CTOR_LIST__ = .;
        *(.init_array)
        __CTOR_END__ = .;

        /* 析构函数列表。 */
        __DTOR_LIST__ = .;
        *(.dtors)
        __DTOR_END__ = .;

        . = ALIGN(4096);
    }

    .bss :
    {
        *(.bss)
        . = ALIGN(4096);
    }
}
