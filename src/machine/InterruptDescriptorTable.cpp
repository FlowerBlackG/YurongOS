/*
 * 中断描述符表相关。
 * 创建于 2022年7月15日。
 */

#include <yros/machine/InterruptDescriptorTable.h>



void InterruptDescriptorTable::storeIdt(IdtRegister& idtr) {
    __asm (
        "sidt %0"
        : "=m" (idtr)
    );
}

void InterruptDescriptorTable::loadIdt(const IdtRegister& idtr) {
    __asm (
        "lidt %0"
        : /* null */
        : "m" (idtr)
    );
}
