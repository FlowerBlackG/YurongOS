/*
 * 中断描述符表相关。
 * 创建于 2022年7月15日。
 */

#pragma once

#include <sys/types.h>

/**
 * IDT 寄存器。
 */
struct IdtRegister {
    uint16_t limit;
    uint32_t baseAddress;
} __packed;

/**
 * 中断门描述符。
 */
struct GateDescriptor {
    uint16_t offsetLow : 16;
    uint16_t segmentSelector : 16;
    uint8_t reserved : 8;

    /**
     * 门类型。
     * 0101b：任务门。很复杂，很低效，在x64下被去掉。
     * 1110b：中断门。会自动将 IF 位置 0。
     * 1111b：陷阱门（trap）。可能引发用户态和内核态的转换。
     */
    uint8_t gateType : 4;

    uint8_t zero : 1;

    uint8_t descriptorPrivilegeLevel : 2;
    uint8_t present : 1;
    uint16_t offsetHigh : 16;
} __packed;

/**
 * 中断向量表。
 */
class InterruptDescriptorTable {
public:
    static void storeIdt(IdtRegister& idtr);
    static void loadIdt(const IdtRegister& idtr);

    GateDescriptor& getDescriptor(size_t idx) {
        return descriptors[idx];
    };
    
    static const int DESCRIPTOR_COUNT = 256;

private:
    GateDescriptor descriptors[DESCRIPTOR_COUNT];
};

#if 1

inline static void __sizetest() {
    int x = sizeof(InterruptDescriptorTable);
}

#endif
