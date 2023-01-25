/*

    内存管理器。

    创建于 2023年1月5日

*/

#pragma once

#include <sys/types.h>
#include <memory/FreeMemoryManager.h>

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

class MemoryManager {

public:
    static const unsigned long ADDRESS_OF_PHYSICAL_MEMORY_MAP = 0xFFFFC00000000000UL;
    static const unsigned long BEGIN_OF_USABLE_ADDRESS = 0xF00000;

public:

    void init();
    static MemoryManager& getInstance() {
        return instance;
    }

    void processArds();

    uint32_t getArdsCount() {
        return * (uint32_t*) (0x500);
    }

    Ards* getArdsBuffer() {
        return (Ards*) (0x508);
    }

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


