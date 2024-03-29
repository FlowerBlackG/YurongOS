// SPDX-License-Identifier: MulanPSL-2.0

/*

    内存管理器。

    创建于 2023年1月5日

*/

#pragma once

#include <lib/sys/types.h>
#include <memory/FreeMemoryManager.h>
#include <memory/PageDirectories.h>
namespace memory {
/**
 * 内存描述符结构。在启动阶段，从 bios 读取。
 */
struct Ards {
    uint64_t base;
    uint64_t size;

    /**
     * 
     * 1：可用。
     * 其他：不可用。
     */
    uint32_t type;
    uint32_t padding;
} __packed;

/**
 * 内存管理器。
 */
namespace MemoryManager {

    const uintptr_t ADDRESS_OF_PHYSICAL_MEMORY_MAP = 0xFFFFC00000000000UL;
    const uintptr_t BEGIN_OF_USABLE_ADDRESS = 0xF00000;  // 15MB
    const int PAGE_SIZE = 4096;

    /**
     * 用户栈基地址。
     * 栈是从高往低生长的，因此大于该地址的内存不应被访问到，
     * 用户栈的实际可访问地址小于该地址值。
     */
    const unsigned long USER_STACK_BASE = 0x800000000000;

    /** 用户栈最大可以拓展到 96 MB。 */
    const unsigned long USER_STACK_SIZE = 96 * 1024 * 1024;
    const uintptr_t KERNEL_PROCESS_STACK_BASE = 0xFFFFC00000000000;
    const uintptr_t KERNEL_PROCESS_STACK_TOP =  0xFFFFBFFFFFA00000;
    const unsigned long KERNEL_PROCESS_STACK_SIZE = KERNEL_PROCESS_STACK_BASE - KERNEL_PROCESS_STACK_TOP;
    const uintptr_t KERNEL_PML4_ADDRESS = 0x1000;


    void init();

    __force_inline PageMapLevel4 getKernelPml4() {
        return reinterpret_cast<PageMapLevel4>(
            KERNEL_PML4_ADDRESS + ADDRESS_OF_PHYSICAL_MEMORY_MAP
        );
    }

    void processArds();

    __force_inline uint32_t getArdsCount() {
        return * (uint32_t*) (0x500 + ADDRESS_OF_PHYSICAL_MEMORY_MAP);
    }

    __force_inline Ards* getArdsBuffer() {
        return (Ards*) (0x508 + ADDRESS_OF_PHYSICAL_MEMORY_MAP);
    }

    uint64_t allocPage(uint64_t count = 1);
    uint64_t allocWhitePage(uint64_t count = 1, uint8_t fill = 0);
    void freePage(uint64_t addr, uint64_t count = 1);

    int mapPage(
        PageMapLevel4 pml4, 
        uintptr_t linearAddress, 
        uintptr_t pagePhysicalAddress,
        bool rw,
        bool us = 1
    );

    enum class WalkPageTablesCommand {
        SKIP_THIS_ENTRY,
        BACK_TO_UPPER,
        WALK_INTO
    };


    /* walk page tables 调用模板：
    
        MemoryManager::walkPageTables(
            (PageMapLevel4) task->pml4Address,
            cargo

            [] (const int64_t cargo, PageMapLevel4 pml4, PageMapLevel4Entry& pml4e) {
                return MemoryManager::WalkPageTablesCommand::SKIP_THIS_ENTRY;
            },

            [] (
                const int64_t cargo,
                PageMapLevel4 pml4, PageMapLevel4Entry& pml4e,
                PageMapLevel3 pml3, PageMapLevel3Entry& pml3e
            ) {
                return MemoryManager::WalkPageTablesCommand::SKIP_THIS_ENTRY;
            },

            [] (
                const int64_t cargo,
                PageMapLevel4 pml4, PageMapLevel4Entry& pml4e,
                PageMapLevel3 pml3, PageMapLevel3Entry& pml3e,
                PageMapLevel2 pml2, PageMapLevel2Entry& pml2e
            ) {
                return MemoryManager::WalkPageTablesCommand::SKIP_THIS_ENTRY;
            },

            [] (
                const int64_t cargo,
                PageMapLevel4 pml4, PageMapLevel4Entry& pml4e,
                PageMapLevel3 pml3, PageMapLevel3Entry& pml3e,
                PageMapLevel2 pml2, PageMapLevel2Entry& pml2e,
                PageMapLevel1 pml1, PageMapLevel1Entry& pml1e
            ) {
                return MemoryManager::WalkPageTablesCommand::SKIP_THIS_ENTRY;
            }
        );
    
    */

    /**
     * 页表 DFS 遍历。
     */
    void walkPageTables(
        PageMapLevel4 pml4,
        const int64_t cargo,

        WalkPageTablesCommand (* onPml4EntryDiscovered) (
            const int64_t cargo,
            PageMapLevel4, PageMapLevel4Entry&
        ),

        WalkPageTablesCommand (* onPml3EntryDiscovered) (
            const int64_t cargo,
            PageMapLevel4, PageMapLevel4Entry&, 
            PageMapLevel3, PageMapLevel3Entry&
        ),

        WalkPageTablesCommand (* onPml2EntryDiscovered) (
            const int64_t cargo,
            PageMapLevel4, PageMapLevel4Entry&, 
            PageMapLevel3, PageMapLevel3Entry&, 
            PageMapLevel2, PageMapLevel2Entry&
        ),

        WalkPageTablesCommand (* onPml1EntryDiscovered) (
            const int64_t cargo,
            PageMapLevel4, PageMapLevel4Entry&, 
            PageMapLevel3, PageMapLevel3Entry&, 
            PageMapLevel2, PageMapLevel2Entry&, 
            PageMapLevel1, PageMapLevel1Entry&
        )
    );

    /**
     * 系统总内存。
     * 含所有内存，包括不让使用的。
     */
    extern uint64_t systemTotalMemory;

    /**
     * 总共管控内存。
     * 所有可写内存，含crt显存映射。
     */
    extern uint64_t systemManagedMemory;

    /**
     * 所有可以由系统自由控制的内存。
     * 不包含crt显存映射，因为它不能乱写。
     * 不含 kernel 二进制程序区，因为不能乱写。
     */
    extern uint64_t systemManagedMildMemory;

};

}