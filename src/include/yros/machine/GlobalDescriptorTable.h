/*
 * 全局描述符表相关内容。
 * 包含段选择子等。
 * 创建于 2022年7月14日。
 */

#pragma once

#include <sys/types.h>

/**
 * 段选择子。
 */
struct SegmentSelector {
    /** 特权级。0为内核，3为用户。 */
    uint8_t requestedPrivilegeLevel : 2;

    /** 查找哪张表。1表示查找LDT，0表示查找GDT。 */
    uint8_t tableIndicator : 1;
    uint16_t index : 13;
} __packed;

/**
 * 段描述符。
 */
struct SegmentDescriptor {
    uint16_t limitLow : 16;
    uint16_t baseLow : 16;

    uint8_t baseMid : 8;

    // access byte

    uint8_t type : 4;

    /** 0表示系统段，1表示代码/数据段。 */
    uint8_t descriptorType : 1;
    uint8_t descriptorPrivilegeLevel : 2;
    uint8_t present : 1;

    uint8_t limitHigh : 4;

    // flags

    uint8_t reserved : 1;

    /** 表示是否使用64位模式。 */
    uint8_t longMode : 1;

    /** 0表示16位保护模式段。1表示32位保护模式段。 */
    uint8_t sizeFlag : 1;

    /** 粒度。0表示1byte，1表示4KB，用于描述 limit 的粒度。 */
    uint8_t granularity : 1;


    uint8_t baseHigh : 8;

} __packed;

struct GdtRegister {
    uint16_t limit : 16;
    uint32_t baseAddress : 32;
} __packed;

/**
 * 全局描述符表（GDT）。
 */
class GlobalDescriptorTable {
public:
    static void storeGDT(GdtRegister& gdtr);
    static void loadGDT(const GdtRegister& gdtr);
private:
    SegmentDescriptor descriptors[16];
};
