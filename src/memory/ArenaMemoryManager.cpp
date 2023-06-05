/*

    arena 内存管理器。

    创建于 2023年2月2日 江西省上饶市玉山县

*/

#include <memory/ArenaMemoryManager.h>
#include <memory/MemoryManager.h>

ArenaBlockNode* ArenaStage::getBlock(uint32_t index) {
    if (index >= descriptor->blocksPerPage) {
        return nullptr;
    }

    uint8_t* addr = reinterpret_cast<uint8_t*>(this + 1);
    addr += index * descriptor->blockSize;

    return reinterpret_cast<ArenaBlockNode*>(addr);
}

namespace ArenaMemoryManager {

    ArenaDescriptor arenaDescriptors[DESCRIPTOR_COUNT];

    void init() {

        uint16_t blockSize = MINIMUM_BLOCK_SIZE;
        for (int i = 0; i < DESCRIPTOR_COUNT; i++) {
            auto& descriptor = arenaDescriptors[i];

            descriptor.blockSize = blockSize;
            descriptor.firstFreeBlock = nullptr;
            descriptor.blocksPerPage = (MemoryManager::PAGE_SIZE - sizeof(ArenaStage)) / blockSize;

            blockSize *= 2;
        }

    }

    ArenaStage* getStage(void* memoryAddr) {
        return reinterpret_cast<ArenaStage*> (uint64_t(memoryAddr) & 0xfffffffffffff000);
    }



}
