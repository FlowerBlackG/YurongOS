/*
 * 硬件控制。
 * 创建于 2022年7月15日。
 */

#include <yros/machine/Machine.h>
#include <stdint.h>
#include <string.h>
#include <CRT.h>

Machine::Machine() {

}

void Machine::init() {

    this->initGDT();
    this->initIDT();



}

void Machine::initGDT() {
    // 将原来的 gdt 换成内核代码里设置的 gdt。
    GlobalDescriptorTable::storeGDT(this->gdtr);

    memcpy(&gdt, (void*) gdtr.baseAddress, gdtr.limit + 1);
    this->gdtr.baseAddress = (uint32_t) &this->gdt;
    this->gdtr.limit = sizeof(gdt) - 1;

    GlobalDescriptorTable::loadGDT(this->gdtr);

    CRT::getInstance().write("info: gdt reloaded.\n");
}

void test() {
    CRT::getInstance().write("warning: int here!\n");
    while (true)
    ;
}

void Machine::initIDT() {

    size_t descriptorCount = sizeof(idt) / sizeof(GateDescriptor);
    for (size_t i = 0; i < descriptorCount; i++) {
        auto& gate = idt.getDescriptor(i);
        gate.offsetLow = (uint32_t)test & 0xffff;
        gate.offsetHigh = ((uint32_t)test >> 16) & 0xffff;
        gate.segmentSelector = 1 << 3;
        gate.reserved = 0;
        gate.zero = 0;
        gate.gateType = 0b1110; // 中断门。
        gate.descriptorPrivilegeLevel = 0;
        gate.present = 1;
    }

    idtr.baseAddress = (uint32_t) &idt;
    idtr.limit = sizeof(idt) - 1;

    InterruptDescriptorTable::loadIDT(idtr);
    CRT::getInstance().write("info: idt loaded.\n");
}


