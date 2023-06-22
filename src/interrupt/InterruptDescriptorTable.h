// SPDX-License-Identifier: MulanPSL-2.0

/*
 * 中断描述符表相关。
 * 创建于 2022年7月15日。
 */

#pragma once

#include <lib/sys/types.h>

/**
 * IDT 寄存器。
 */
struct IdtRegister {
    uint16_t limit;
    uint64_t baseAddress;
} __packed;

/**
 * 中断门描述符。
 */
struct GateDescriptor {
    uint64_t offsetLow : 16;
    uint64_t segmentSelector : 16;
    uint64_t ist : 3;
    uint64_t reserved0 : 5;

    /**
     * 门类型。
     * 0101b：任务门。很复杂，很低效，在x64下被去掉。
     * 1110b：中断门。会自动将 IF 位置 0。
     * 1111b：陷阱门（trap）。可能引发用户态和内核态的转换。
     */
    uint64_t gateType : 4;

    uint64_t zero : 1;

    uint64_t descriptorPrivilegeLevel : 2;
    uint64_t present : 1;
    uint64_t offsetMiddle : 16;

    uint64_t offsetHigh : 32;
    uint64_t reserved1 : 32;
} __packed;

/**
 * 中断向量表。
 */
class InterruptDescriptorTable {
public:
    static void storeIdt(IdtRegister& idtr);
    static void loadIdt(const IdtRegister& idtr);

    void setInterruptGate(int id, uint64_t handlerAddress);
    void setTrapGate(int id, uint64_t handlerAddress);

    GateDescriptor& getDescriptor(size_t idx) {
        return descriptors[idx];
    };
    
    static const int DESCRIPTOR_COUNT = 256;

private:
    GateDescriptor descriptors[DESCRIPTOR_COUNT];
};


