/*
 * 全局描述符表相关内容。
 * 包含段选择子等。
 * 创建于 2022年7月14日。
 */

#pragma once

#include <lib/sys/types.h>

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

    const static int SEGMENT_TYPE_CODE_DATA = 1;

    uint16_t limitLow;
    uint16_t baseLow;

    uint8_t baseMid;

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


    uint8_t baseHigh;

} __packed;

struct SystemSegmentDescriptor {

    const static int SEGMENT_TYPE_SYSTEM = 0;

    const static int SYSTEM_SEGMENT_TYPE_LDT = 0x2;
    const static int SYSTEM_SEGMENT_TYPE_64_BIT_AVAILABLE = 0x9;
    const static int SYSTEM_SEGMENT_TYPE_64_BIT_BUSY = 0xB;

    uint16_t limitLow;
    uint16_t baseLow;

    uint8_t baseMid;

    // access byte

    uint8_t systemSegmentType : 4;

    /** 0表示系统段，1表示代码/数据段。 */
    uint8_t descriptorType : 1;
    uint8_t privilegeLevel : 2;
    uint8_t present : 1;

    uint8_t limitHigh : 4;

    // flags

    uint8_t reserved0 : 1;

    /** 表示是否使用64位模式。 */
    uint8_t longMode : 1;

    /** 0表示16位保护模式段。1表示32位保护模式段。 */
    uint8_t sizeFlag : 1;

    /** 粒度。0表示1byte，1表示4KB，用于描述 limit 的粒度。 */
    uint8_t granularity : 1;


    uint8_t baseHigh;

    uint32_t baseUltraHigh;
    uint32_t reserved1;

} __packed;

struct GdtRegister {
    uint16_t limit;
    uint64_t baseAddress;
} __packed;

struct TaskStateSegment {
    uint32_t reserved0;
    
    uint32_t rsp0Low;
    uint32_t rsp0High;
    uint32_t rsp1Low;
    uint32_t rsp1High;
    uint32_t rsp2Low;
    uint32_t rsp2High;

    uint32_t reserved1;
    uint32_t reserved2;

    uint32_t ist1Low;
    uint32_t ist1High;
    uint32_t ist2Low;
    uint32_t ist2High;
    uint32_t ist3Low;
    uint32_t ist3High;
    uint32_t ist4Low;
    uint32_t ist4High;
    uint32_t ist5Low;
    uint32_t ist5High;
    uint32_t ist6Low;
    uint32_t ist6High;
    uint32_t ist7Low;
    uint32_t ist7High;

    uint32_t reserved3;
    uint32_t reserved4;

    uint16_t reserved5;
    uint16_t iopb;

} __packed;

/**
 * 全局描述符表（GDT）。
 */
namespace GlobalDescriptorTable {

    const int KERNEL_CODE_DESCRIPTOR_IDX = 1;
    const int KERNEL_DATA_DESCRIPTOR_IDX = 2;
    const int USER_CODE_32_DESCRIPTOR_IDX = 3;
    const int USER_DATA_DESCRIPTOR_IDX = 4;
    const int USER_CODE_64_DESCRIPTOR_IDX = 5;
    const int TASK_STATE_SEGMENT_DESCRIPTOR_IDX = 6;

    const int KERNEL_CODE_SELECTOR = KERNEL_CODE_DESCRIPTOR_IDX << 3;
    const int KERNEL_DATA_SELECTOR = KERNEL_DATA_DESCRIPTOR_IDX << 3;

    const int USER_CODE_32_SELECTOR = (USER_CODE_32_DESCRIPTOR_IDX << 3) | 0x3;
    const int USER_DATA_SELECTOR = (USER_DATA_DESCRIPTOR_IDX << 3) | 0x3;
    const int USER_CODE_64_SELECTOR = (USER_CODE_64_DESCRIPTOR_IDX << 3) | 0x3;

    const int TASK_STATE_SEGMENT_SELECTOR = TASK_STATE_SEGMENT_DESCRIPTOR_IDX << 3;

    void init();

    void initTaskStateSegment();

    void storeGdt(GdtRegister& gdtr);
    void loadGdt(const GdtRegister& gdtr);

    struct Descriptors {
        SegmentDescriptor zero;
        SegmentDescriptor kernelCode;
        SegmentDescriptor kernelData;
        SegmentDescriptor userCode32;
        SegmentDescriptor userData;
        SegmentDescriptor userCode64;
        SystemSegmentDescriptor systemSegmentDescriptor;
    } __packed;

    extern Descriptors descriptors;
    extern TaskStateSegment taskStateSegment;
};

#if 0
static void __check_size() {
    sizeof(GdtRegister);
    sizeof(SegmentDescriptor);
    sizeof(SystemSegmentDescriptor);
    static_assert(sizeof(SystemSegmentDescriptor) == 128 / 8);
    static_assert(8 == sizeof(SegmentDescriptor));
    sizeof(TaskStateSegment);
    static_assert(sizeof(TaskStateSegment) == 0x68);
}
#endif
