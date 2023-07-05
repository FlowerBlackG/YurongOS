// SPDX-License-Identifier: MulanPSL-2.0

/*
 * 硬件控制。
 * 创建于 2022年7月15日。
 */

#include <machine/Machine.h>
#include <lib/stdint.h>
#include <lib/string.h>
#include <crt/CRT.h>
#include <misc/io.h>
#include <lib/stdio.h>
#include <machine/X86Assembly.h>
#include <interrupt/InterruptHandlers.h>
#include <interrupt/ClockInterrupt.h>
#include <interrupt/KeyboardInterrupt.h>
#include <interrupt/PageFaultException.h>
#include <memory/MemoryManager.h>
#include <device/acpi/acpi.h>

Machine Machine::instance;

Machine::Machine() {

}

void Machine::init() {

    GlobalDescriptorTable::init();

    // 需要在设置中断前初始化内存。
    // 内存初始化过程中，会依赖 bios 提供的中断程序。
    memory::MemoryManager::init();

    this->initIdt();

    /*
     * 初始化 ACPI
     * 该过程包含对 PCIe 的初始化。
     */
    device::acpi::init();

    this->initPit();
    this->initPic();

}


void Machine::setInterruptState(bool enabled) {
    if (enabled) {
        x86asmSti();
    } else {
        x86asmCli();
    }
}


bool Machine::getAndSetInterruptState(bool enabled) {
    uint64_t rax;

    __asm (
        "pushfq \n"
        "popq %%rax \n"
        "shrq $9, %%rax \n"
        "andq $0x1, %%rax"
        : "=a" (rax)
        :
    );

    if (enabled) {
        x86asmSti();
    } else {
        x86asmCli();
    }

    return rax;
}

bool Machine::getInterruptState() {

    uint64_t rax;

    __asm (
        "pushfq \n"
        "popq %%rax \n"
        "shrq $9, %%rax \n"
        "andq $0x1, %%rax"
        : "=a" (rax)
        :
    );

    return rax;
}

void Machine::setCR3(uint64_t pml4Addr) {
    __asm (
        "movq %%rax, %%cr3"
        :
        : "a" (pml4Addr)
    );
}

uint64_t Machine::getCR2() {
    uint64_t res;

    __asm (
        "movq %%cr2, %%rax" 
        : "=a" (res) 
        :
    );

    return res;
}

uint64_t Machine::getCR3() {
    uint64_t res;

    __asm (
        "movq %%cr3, %%rax" 
        : "=a" (res) 
        :
    );

    return res;
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

    idt.setTrapGate(14, (long) PageFaultException::entrance);
    
    idt.setTrapGate(16, (long) InterruptHandlers::mathFaultExceptionEntrance);
    idt.setTrapGate(17, (long) InterruptHandlers::alignmentCheckingExceptionEntrance);
    idt.setTrapGate(18, (long) InterruptHandlers::machineCheckExceptionEntrance);
    idt.setTrapGate(19, (long) InterruptHandlers::extendedMathFaultExceptionEntrance);
    idt.setTrapGate(20, (long) InterruptHandlers::virtualizationExceptionExceptionEntrance);
    idt.setTrapGate(21, (long) InterruptHandlers::controlProtectionExceptionExceptionEntrance);


    idt.setInterruptGate(0x20, (long) ClockInterrupt::entrance);
    idt.setInterruptGate(0x21, (long) KeyboardInterrupt::entrance);

    IdtRegister idtr;

    idtr.baseAddress = uint64_t(&idt);
    idtr.limit = sizeof(idt) - 1;

    InterruptDescriptorTable::loadIdt(idtr);
    CRT::getInstance().write("[info] idt loaded.\n");
}



void Machine::initPic() {
    io::outByte(PIC_MASTER_CTRL, 0b00010001);
    io::outByte(PIC_MASTER_DATA, 0x20);
    io::outByte(PIC_MASTER_DATA, 0b00000100);
    io::outByte(PIC_MASTER_DATA, 0b00000001);

    io::outByte(PIC_SLAVE_CTRL, 0b00010001);
    io::outByte(PIC_SLAVE_DATA, 0x28);
    io::outByte(PIC_SLAVE_DATA, 2);
    io::outByte(PIC_SLAVE_DATA, 0b00000001);

    // io::outByte(PIC_MASTER_DATA, 0b11111100);
    io::outByte(PIC_MASTER_DATA, 0b11111111); // 屏蔽 pic 的所有中断。使用 apic 控制中断。
    io::outByte(PIC_SLAVE_DATA, 0b11111111);

}

void Machine::initPit() {

    /*
    
        0x43: 控制端口
        0x40: 频道0数据端口

        0x43 指令：
          bits

          [0, 0]: 0 = 16-bit binary, 1 = 4-digit BCD

          [1, 3]: operating mode
                  0 1 1 = square wave generator

          [4, 5]: access mode
                  1 1 = low byte then high byte

          [6, 7]: select channel
                  0 0 = channel 0
                  0 1 = channel 1
                  1 0 = channel 2    

        ref: https://wiki.osdev.org/Programmable_Interval_Timer
    */

    // todo: 关闭 pit 芯片。使用 hpet 产生时钟信号。
    // io::outByte(0x43, 0b00110110);
    // io::outByte(0x40, ClockInterrupt::CLOCK_COUNTER & 0xff);
    // io::outByte(0x40, (ClockInterrupt::CLOCK_COUNTER >> 8) & 0xff);
}
