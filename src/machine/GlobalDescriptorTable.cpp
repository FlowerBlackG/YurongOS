/*
 * 全局描述符表相关内容。
 * 包含段选择子等。
 * 创建于 2022年7月15日。
 */

#include <yros/machine/GlobalDescriptorTable.h>


void GlobalDescriptorTable::storeGDT(GdtRegister& gdtr) {
    __asm (
        "sgdt %0"
        : "=m" (gdtr)
    );
}

void GlobalDescriptorTable::loadGDT(const GdtRegister& gdtr) {
    __asm (
        "lgdt %0"
        : /* null */
        : "m" (gdtr)
    );
}
