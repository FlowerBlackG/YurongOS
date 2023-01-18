/*
 * 硬件控制。
 * 创建于 2022年7月15日。
 */

#include <machine/Machine.h>
#include <stdint.h>
#include <string.h>
#include <CRT.h>
#include <yros/IO.h>
#include <stdio.h>
#include <machine/X86Assembly.h>
#include <machine/InterruptHandlers.h>
#include <machine/MemoryManager.h>

Machine::Machine() {

}

void Machine::init() {

    this->initPic();
    this->initPit();
    this->initGdt();

    // 需要在设置中断前初始化内存。
    // 内存初始化过程中，会依赖 bios 提供的中断程序。
    MemoryManager::getInstance().init();

    this->initIdt();

}

void Machine::initGdt() {
    // 将原来的 gdt 换成内核代码里设置的 gdt。
    GlobalDescriptorTable::storeGdt(this->gdtr);

    memcpy(&gdt, (void*) gdtr.baseAddress, gdtr.limit + 1);
    this->gdtr.baseAddress = (uint64_t) &this->gdt;
    this->gdtr.limit = sizeof(gdt) - 1;

    GlobalDescriptorTable::loadGdt(this->gdtr);

    CRT::getInstance().write("info: gdt reloaded.\n");
}

void test() {
    CRT::getInstance().write("warning: int here!\n");
    while (true)
    ;
}

int counter = 0;
/* 该函数以后需要删除。 */
void default_handler(int vector) {

    if (vector >= 0x20 && vector < 0x28) {
    
        IO::outByte(Machine::PIC_MASTER_CTRL, Machine::PIC_EOI);
    
    } else if (vector >= 0x28 && vector < 0x30) {
    
        IO::outByte(Machine::PIC_MASTER_CTRL, Machine::PIC_EOI);
        IO::outByte(Machine::PIC_SLAVE_CTRL, Machine::PIC_EOI);
    
    }

    counter++;
    char s[128];
    sprintf(s, "vec: %x, counter: %d\n", vector, counter);
    CRT::getInstance().write(s);

    
}

void Machine::initIdt() {


    idt.setTrapGate(0, (long) InterruptHandlers::divideErrorExceptionEntrance);
    idt.setTrapGate(1, (long) InterruptHandlers::debugExceptionEntrance);
    idt.setTrapGate(2, (long) InterruptHandlers::nonMaskableInterruptExceptionEntrance);
    idt.setTrapGate(3, (long) InterruptHandlers::breakpointExceptionEntrance);
    idt.setTrapGate(4, (long) InterruptHandlers::overflowExceptionEntrance);
    idt.setTrapGate(5, (long) InterruptHandlers::boundaryRangeExceededExceptionEntrance);
    idt.setTrapGate(6, (long) InterruptHandlers::undefinedOpcodeExceptionEntrance);
    idt.setTrapGate(7, (long) InterruptHandlers::deviceNotAvailableExceptionEntrance);
    idt.setTrapGate(8, (long) InterruptHandlers::doubleFaultExceptionEntrance);
    idt.setTrapGate(10, (long) InterruptHandlers::invalidTssExceptionEntrance);
    idt.setTrapGate(11, (long) InterruptHandlers::notPresentExceptionEntrance);
    idt.setTrapGate(12, (long) InterruptHandlers::stackSegmentExceptionEntrance);
    idt.setTrapGate(13, (long) InterruptHandlers::generalProtectionExceptionEntrance);
    idt.setTrapGate(14, (long) InterruptHandlers::pageFaultExceptionEntrance);
    idt.setTrapGate(16, (long) InterruptHandlers::mathFaultExceptionEntrance);
    idt.setTrapGate(17, (long) InterruptHandlers::alignmentCheckingExceptionEntrance);
    idt.setTrapGate(18, (long) InterruptHandlers::machineCheckExceptionEntrance);
    idt.setTrapGate(19, (long) InterruptHandlers::extendedMathFaultExceptionEntrance);
    idt.setTrapGate(20, (long) InterruptHandlers::virtualizationExceptionExceptionEntrance);
    idt.setTrapGate(21, (long) InterruptHandlers::controlProtectionExceptionExceptionEntrance);


    idt.setInterruptGate(0x20, (uint64_t)InterruptHandlers::clockInterruptEntrance);

    idtr.baseAddress = (uint64_t) &idt;
    idtr.limit = sizeof(idt) - 1;

    InterruptDescriptorTable::loadIdt(idtr);
    CRT::getInstance().write("info: idt loaded.\n");
}



void Machine::initPic() {
    IO::outByte(PIC_MASTER_CTRL, 0b00010001);
    IO::outByte(PIC_MASTER_DATA, 0x20);
    IO::outByte(PIC_MASTER_DATA, 0b00000100);
    IO::outByte(PIC_MASTER_DATA, 0b00000001);

    IO::outByte(PIC_SLAVE_CTRL, 0b00010001);
    IO::outByte(PIC_SLAVE_DATA, 0x28);
    IO::outByte(PIC_SLAVE_DATA, 2);
    IO::outByte(PIC_SLAVE_DATA, 0b00000001);

    IO::outByte(PIC_MASTER_DATA, 0b11111110);
    IO::outByte(PIC_SLAVE_DATA, 0b11111110);

}

void Machine::initPit() {
    IO::outByte(0x43, 0b00110100);
    IO::outByte(0x40, InterruptHandlers::CLOCK_COUNTER & 0xff);
    IO::outByte(0x40, (InterruptHandlers::CLOCK_COUNTER >> 8) & 0xff);
}
