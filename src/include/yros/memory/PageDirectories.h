/*

    内存分页结构。

    创建于2023年1月3日，安徽桐城

*/

#pragma once

#include <lib/sys/types.h>

/*

    四级页表结构
      x64 架构使用四级页表映射内存。
      此时，分段功能被关闭（强制使用平铺模型）

      虚拟内存寻址空间为 48 位，总共 256TB
      通过四级页表，分别管理：
        [0, 12): 物理地址。每页框 4KB。
        [12, 21): page table entry
        [21, 30): page middle directory
        [30, 39): page upper directory
        [39, 48): page global directory

      每单张表都需要 4KB

*/

struct PageTableEntry {

    // byte 0

    uint64_t present : 1;
    uint64_t rw : 1;

    /** user(1) / supervisor(0) */
    uint64_t us : 1;

    uint64_t writeThrough : 1;
    uint64_t cacheDisable : 1;
    uint64_t accessed : 1;
    uint64_t dirty : 1;
    uint64_t pageAttributeTable : 1;

    // byte 1 to 7

    uint64_t global : 1;
    uint64_t available0 : 3;
    uint64_t pageFrameNumber : 36;
    uint64_t reserved : 4;
    uint64_t available1 : 7;
    uint64_t protectionKey : 4;
    uint64_t executeDisable : 1;

} __packed;

typedef PageTableEntry PageMapLevel1Entry;

struct PageDirectoryEntry {
    uint64_t present : 1;
    uint64_t rw : 1;

    /** user(1) / supervisor(0) */
    uint64_t us : 1;

    uint64_t writeThrough : 1;
    uint64_t cacheDisable : 1;
    uint64_t accessed : 1;
    uint64_t available0 : 1;
    uint64_t pageSize : 1;

    uint64_t available1 : 4;
    uint64_t pageFrameNumber : 36;
    uint64_t reserved : 4;
    uint64_t available2 : 11;
    uint64_t executeDisable : 1;
} __packed;

typedef PageDirectoryEntry PageDirectoryPointerEntry;

typedef PageDirectoryEntry PageMapLevel2Entry;
typedef PageDirectoryPointerEntry PageMapLevel3Entry;

/**
 * Page Map Level 4 Entry.
 */
struct PageMapGlobalEntry {

    // byte 0

    uint64_t present : 1;
    uint64_t rw : 1;

    /** user(1) / supervisor(0) */
    uint64_t us : 1;

    uint64_t writeThrough : 1;
    uint64_t cacheDisable : 1;
    uint64_t accessed : 1;
    uint64_t available0 : 1;
    uint64_t reserved0 : 1;

    // byte 1 to 5

    uint64_t available1 : 4;
    uint64_t pageFrameNumber : 36;

    // byte 6, 7

    uint64_t reserved1 : 4;
    uint64_t available2 : 11;
    uint64_t executeDisable : 1;
} __packed;

typedef PageMapGlobalEntry PageMapLevel4Entry;

typedef PageMapLevel1Entry* PageMapLevel1;
typedef PageMapLevel2Entry* PageMapLevel2;
typedef PageMapLevel3Entry* PageMapLevel3;
typedef PageMapLevel4Entry* PageMapLevel4;

#if 1
static void __check_size() {
    sizeof(PageTableEntry);
    sizeof(PageDirectoryEntry);
}
#endif
