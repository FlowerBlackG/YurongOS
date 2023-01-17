/*
 * 中断描述符表相关。
 * 创建于 2022年7月15日。
 */

#include <machine/InterruptDescriptorTable.h>



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

void InterruptDescriptorTable::setInterruptGate(int id, uint32_t handlerAddress) {
    auto&& gate = descriptors[id];

    gate.offsetLow = (uint32_t) handlerAddress & 0xffff;
    gate.offsetHigh = ((uint32_t) handlerAddress >> 16) & 0xffff;
    gate.segmentSelector = 1 << 3;
    gate.reserved = 0;
    gate.zero = 0;
    gate.gateType = 0b1110; // 清 IF 位。
    gate.descriptorPrivilegeLevel = 3;
    gate.present = 1;
}

void InterruptDescriptorTable::setTrapGate(int id, uint32_t handlerAddress) {
    auto&& gate = descriptors[id];

    gate.offsetLow = (uint32_t) handlerAddress & 0xffff;
    gate.offsetHigh = ((uint32_t) handlerAddress >> 16) & 0xffff;
    gate.segmentSelector = 1 << 3;
    gate.reserved = 0;
    gate.zero = 0;
    gate.gateType = 0b1111; // 不清 IF 位。 
    gate.descriptorPrivilegeLevel = 3;
    gate.present = 1;
}
