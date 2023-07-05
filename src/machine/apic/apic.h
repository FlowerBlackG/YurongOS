// SPDX-License-Identifier: MulanPSL-2.0

/*
 * Advanced Programmable Interrupt Controller.
 *
 * 创建于 2023年7月3日 上海市嘉定区安亭镇
 */

#pragma once

#include <lib/cpuid.h>
#include <lib/collections/ArrayList.hpp>
#include <misc/Kernel.h>

namespace machine {
namespace apic {

/**
 *
 * 参考：
 *   https://wiki.osdev.org/IOAPIC
 */
class IOApic {
protected:

    uint8_t ioApicId;
    uint8_t version;
    uint32_t ioApicAddr;
    uint32_t globalSystemInterruptBase;
    intptr_t ioApicLinearAddr;
    int32_t redirectionEntryCount;

public:

    union RedirectionEntry {
        struct {
            int64_t vector : 8;
            int64_t deliveryMode : 3;
            int64_t destinationMode : 1;
            int64_t deliveryStatus : 1;
            int64_t pinPolarity : 1; // ？？？
            int64_t remoteIRR : 1;
            int64_t triggerMode : 1;
            int64_t mask : 1;
            int64_t reserved : 39;
            int64_t destination : 8;
        } __packed;

        struct {
            int32_t lowerDWord;
            int32_t upperDWord;
        } __packed;
    };

    IOApic(uint8_t ioApicId, uint32_t ioApicAddr, uint32_t gsiBase);

    int32_t readRegister(const int8_t offset);
    void writeRegister(const int8_t offset, const int32_t data);

    inline int32_t getRedirectionEntryCount() {
        return this->redirectionEntryCount;
    };

    RedirectionEntry getRedirectionEntry(uint8_t entryNo);
    void writeRedirectionEntry(uint8_t entryNo, RedirectionEntry& entry);

};

extern lib::collections::ArrayList<void*> ioApicList;

inline IOApic* getIOApic(int32_t idx) {
    if (idx >= ioApicList.size() || idx < 0) {
        return nullptr;
    }

    return (IOApic*) ioApicList[idx];
}

/**
 *
 * 线性地址。
 */
extern intptr_t localApicRegisterAddr;

void initLocalAPIC(intptr_t addrOfLAPIC);

/**
 * 发送 end of interrupt 信号。
 */
void localApicEOI();


}
}
