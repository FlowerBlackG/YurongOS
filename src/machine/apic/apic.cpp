// SPDX-License-Identifier: MulanPSL-2.0

/*
 * Advanced Programmable Interrupt Controller.
 *
 * 创建于 2023年7月3日 上海市嘉定区安亭镇
 */

#include "./apic.h"

#include <machine/Msr.h>
#include <lib/collections/Map.hpp>
#include <lib/collections/LinkedList.h>
#include <misc/Kernel.h>
#include <memory/memory.h>

namespace machine {
namespace apic {

lib::collections::ArrayList<void*> ioApicList;
intptr_t localApicRegisterAddr;

IOApic::IOApic(uint8_t ioApicId, uint32_t ioApicAddr, uint32_t gsiBase) {
    this->ioApicAddr = ioApicAddr;
    this->ioApicId = ioApicId;
    this->globalSystemInterruptBase = gsiBase;

    this->ioApicLinearAddr = ioApicAddr + memory::addrOfPhysicalMemoryMap;

    this->version = readRegister(1) >> 24;

    this->redirectionEntryCount = int8_t(this->readRegister(1) >> 16) + 1;

    Kernel::printLog(
        Kernel::LogColor::H_PURPLE,
        "apic ver: %d | 0x%x | redir count: %d\n",
        this->version,
        readRegister(1),
        this->redirectionEntryCount
    );

    // 初始化中断向量信息。
    RedirectionEntry entry;

    entry.upperDWord = 0;
    entry.lowerDWord = 0;
    entry.destination = 0;

    // 时钟
    entry.vector = 0x20;
    this->writeRedirectionEntry(16, entry);

    // 键盘
    entry.vector = 0x21;
    this->writeRedirectionEntry(1, entry);

}

int32_t IOApic::readRegister(const int8_t offset) {
    *(int32_t volatile*) ioApicLinearAddr = offset;
    return *(int32_t volatile*) (ioApicLinearAddr + 0x10);
}

void IOApic::writeRegister(const int8_t offset, const int32_t data) {
    *(int32_t volatile*) ioApicLinearAddr = offset;
    *(int32_t volatile*) (ioApicLinearAddr + 0x10) = data;
}


IOApic::RedirectionEntry IOApic::getRedirectionEntry(uint8_t entryNo) {
    IOApic::RedirectionEntry res;

    res.lowerDWord = this->readRegister(0x10 + entryNo * 2);
    res.upperDWord = this->readRegister(0x10 + entryNo * 2 + 1);

    return res;
}

void IOApic::writeRedirectionEntry(uint8_t entryNo, RedirectionEntry& entry) {
    this->writeRegister(0x10 + entryNo * 2, entry.lowerDWord);
    this->writeRegister(0x10 + entryNo * 2 + 1, entry.upperDWord);
}


void initLocalAPIC(intptr_t addrOfLAPIC) {

    // enable xAPIC
    auto value = Msr::read(Msr::IA32_APIC_BASE);
    // 第10位：enable x2APIC mode
    // 第11位：APIC global enable
    value |= (1 << 11);
    Msr::write(Msr::IA32_APIC_BASE, value);

    value = Msr::read(Msr::IA32_APIC_BASE);

    Kernel::printLog("IA32_APIC_BASE: 0x%llx\n", value);
    if (value & 0x800) {
        Kernel::printLog("  [xAPIC enabled]\n");
    }

    intptr_t linearAddrOfLAPIC = addrOfLAPIC + memory::addrOfPhysicalMemoryMap;
    apic::localApicRegisterAddr = linearAddrOfLAPIC;

    // set spurious interrupt vector register bit 8 to start receive interrupts
    auto sivg = *(int32_t volatile*) (localApicRegisterAddr + 0xF0);
    sivg |= 0x100;
    *(int32_t volatile*) (localApicRegisterAddr + 0xF0) = sivg;

    Kernel::printLog(
        Kernel::LogColor::GREEN,
        "sivg enabled. (0x%x)\n",
        *(int32_t volatile*)(localApicRegisterAddr + 0xF0)
    );
    
}

void localApicEOI() {
    *(int32_t volatile*) (localApicRegisterAddr + 0xB0) = 0;
}


}
}
