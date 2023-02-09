/*

    arena 内存管理器。

    创建于 2023年2月2日 江西省上饶市玉山县

*/


#pragma once

#include <lib/sys/types.h>

struct ArenaBlockNode {
    ArenaBlockNode* prev;
    ArenaBlockNode* next;
};

struct ArenaDescriptor {
    uint32_t blockSize;
    uint32_t blocksPerPage;

    ArenaBlockNode* firstFreeBlock;
};

/**
 * arena 舞台。该结构位于内存页的头部，负责管理该页内的小内存。
 */
struct ArenaStage {
    ArenaDescriptor* descriptor;
     
    uint32_t count;
    uint32_t large;

    ArenaBlockNode* getBlock(uint32_t index);
};

/**
 * arena 内存管理器。
 */
namespace ArenaMemoryManager {
    const int MINIMUM_BLOCK_SIZE = 16;
    const int MAXIMUM_LINKABLE_BLOCK_SIZE = 1024;
    const int DESCRIPTOR_COUNT = 7;
    extern ArenaDescriptor arenaDescriptors[DESCRIPTOR_COUNT];

    void init();
    ArenaStage* getStage(void* memoryAddr);
}


#if 0

static void __check_size() {
    static_assert(16 == sizeof(ArenaStage));
    static_assert(1 << ArenaMemoryManager::DESCRIPTOR_COUNT == 1024 / 16 * 2);
};

#endif
