/*

    内存管理器。

    创建于 2023年1月5日

*/

#pragma once

#include <sys/types.h>

struct Ards {
    uint64_t base;
    uint64_t size;
    uint32_t type;
} __packed;

class MemoryManager {
public:

    void init();
    static MemoryManager& getInstance() {
        return instance;
    }

    void processArds();

    uint32_t getArdsCount() {
        return * (uint32_t*) (0x1000);
    }

    Ards* getArdsBuffer() {
        return (Ards*) (0x2000);
    }

private:

private:


private:
    MemoryManager(const MemoryManager&) = delete;
    MemoryManager() = default;

    static MemoryManager instance;
};


