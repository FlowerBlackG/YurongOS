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

    /** 保留为 0，供 cpu 使用。 */
    uint8_t access : 1;

    /**
     * readable bit or writable bit
     * 
     * 对于代码段，该变量规定指定内容是否可读。
     * 代码段永远不可写。
     * 
     * 对于数据段，该变量规定内容是否可写。
     * 数据段永远可读。
     */
    uint8_t rw : 1;

    /**
     * direction bit or conforming bit
     * 
     * 对于数据段，该值为 0 时表示“向上生长”，为 1 表示“向下生长”。
     * 生长方向表示的是 limit 相对 base 的生长方向。
     * 该“生长”并不会影响运行过程中 esp 的增长方向。
     * （运行时，入栈时 esp 应该是向下增长的）
     */
    uint8_t dc : 1;

    /**
     * 0 表示描述符定义的是数据段。
     * 1 表示描述符表示的段允许被执行。
     */
    uint8_t executable : 1;

    /** 0表示系统段，1表示代码/数据段。 */
    uint8_t descriptorType : 1;
    uint8_t privilegeLevel : 2;
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
    uint16_t limit;
    uint64_t baseAddress;
} __packed;

/**
 * 全局描述符表（GDT）。
 */
class GlobalDescriptorTable {
public:
    static void storeGdt(GdtRegister& gdtr);
    static void loadGdt(const GdtRegister& gdtr);
private:
    SegmentDescriptor descriptors[8];
};

#if 0
static void __check_size() {
    sizeof(GdtRegister);
}
#endif
