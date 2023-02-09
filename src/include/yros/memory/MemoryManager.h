/*

    内存管理器。

    创建于 2023年1月5日

*/

#pragma once

#include <lib/sys/types.h>
#include <yros/memory/FreeMemoryManager.h>
#include <yros/memory/PageDirectories.h>

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
class MemoryManager {

public:
    static const unsigned long ADDRESS_OF_PHYSICAL_MEMORY_MAP = 0xFFFFC00000000000UL;
    static const unsigned long BEGIN_OF_USABLE_ADDRESS = 0xF00000;
    static const int PAGE_SIZE = 4096;

    /**
     * 用户栈基地址。
     * 栈是从高往低生长的，因此大于该地址的内存不应被访问到，
     * 用户栈的实际可访问地址小于该地址值。
     */
    static const unsigned long USER_STACK_BASE = 0x800000000000;

    /** 用户栈最大可以拓展到 96 MB。 */
    static const unsigned long USER_STACK_SIZE = 96 * 1024 * 1024;
    static const unsigned long KERNEL_PROCESS_STACK_BASE = 0xFFFFC00000000000;
    static const unsigned long KERNEL_PROCESS_STACK_TOP =  0xFFFFBFFFFFA00000;
    static const unsigned long KERNEL_PROCESS_STACK_SIZE = KERNEL_PROCESS_STACK_BASE - KERNEL_PROCESS_STACK_TOP;
    static const unsigned long KERNEL_PML4_ADDRESS = 0x1000;


public:

    void init();
    static inline MemoryManager& getInstance() {
        return instance;
    }

    static PageMapLevel4 getKernelPml4() {
        return reinterpret_cast<PageMapLevel4>(
            KERNEL_PML4_ADDRESS + ADDRESS_OF_PHYSICAL_MEMORY_MAP
        );
    }

    void processArds();

    inline static uint32_t getArdsCount() {
        return * (uint32_t*) (0x500 + ADDRESS_OF_PHYSICAL_MEMORY_MAP);
    }

    inline static Ards* getArdsBuffer() {
        return (Ards*) (0x508 + ADDRESS_OF_PHYSICAL_MEMORY_MAP);
    }

    uint64_t allocPage(uint64_t count = 1);
    void freePage(uint64_t addr, uint64_t count = 1);

protected:

    FreeMemoryManager freeMemoryManager;

    /**
     * 系统总内存。
     * 含所有内存，包括不让使用的。
     */
    uint64_t systemTotalMemory;

    /**
     * 总共管控内存。
     * 所有可写内存，含crt显存映射。
     */
    uint64_t systemManagedMemory;

    /**
     * 所有可以由系统自由控制的内存。
     * 不包含crt显存映射，因为它不能乱写。
     * 不含 kernel 二进制程序区，因为不能乱写。
     */
    uint64_t systemManagedMildMemory;


private:
    MemoryManager(const MemoryManager&) = delete;
    MemoryManager() = default;

    static MemoryManager instance;
};

