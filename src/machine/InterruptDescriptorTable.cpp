/*
 * 中断描述符表相关。
 * 创建于 2022年7月15日。
 */

#include <yros/machine/InterruptDescriptorTable.h>



void InterruptDescriptorTable::storeIDT(IdtRegister& idtr) {
    __asm (
        "sidt %0"
        : "=m" (idtr)
    );
}

void InterruptDescriptorTable::loadIDT(const IdtRegister& idtr) {
    __asm (
        "lidt %0"
        : /* null */
        : "m" (idtr)
    );
}
