/*

    内核堆内存管理器。

    创建于 2023年2月2日 江西省上饶市玉山县

*/

#include <memory/MemoryManager.h>
#include <memory/ArenaMemoryManager.h>
#include <memory/KernelMemoryAllocator.h>

#include <yros/Kernel.h>

namespace KernelMemoryAllocator {

    void* malloc(uint64_t size) {

        if (!size) {
            return nullptr;
        }
        
        if (size > ArenaMemoryManager::MAXIMUM_LINKABLE_BLOCK_SIZE) {
            uint64_t requiredSize = size + sizeof(ArenaStage);

            // 按照 4KB 向上取整。计算需要的页数。
            requiredSize += (4096 - 1);
            uint64_t requiredPages = requiredSize /= 4096;
            
            void* page = allocPage(requiredPages);
            if (page == nullptr) {
                Kernel::panic("[error] failed to allocate page.\n");
            }

            ArenaStage* stage = (ArenaStage*) page;
            stage->count = requiredPages;
            stage->large = 1;
            stage->descriptor = nullptr;

            return stage + 1;
        }

        ArenaDescriptor* descriptor;

        for (int idx = 0; idx < ArenaMemoryManager::DESCRIPTOR_COUNT; idx++) {
            descriptor = &ArenaMemoryManager::arenaDescriptors[idx];
            if (descriptor->blockSize >= size) {
                break;
            }
        }

        ArenaBlockNode* list = descriptor->firstFreeBlock;

        if (list == nullptr) {
            ArenaStage* newStage = (ArenaStage*) allocPage();

            if (!newStage) {
                Kernel::panic("[error] failed to alloc page.\n");
            }

            newStage->descriptor = descriptor;
            newStage->large = 0;
            newStage->count = descriptor->blocksPerPage;

            ArenaBlockNode* firstBlock = newStage->getBlock(0);
            descriptor->firstFreeBlock = firstBlock;
            list = firstBlock;
            firstBlock->prev = nullptr;

            for (int idx = 1; idx < newStage->count; idx++) {
                ArenaBlockNode* block = newStage->getBlock(idx);
                block->prev = newStage->getBlock(idx - 1);
                newStage->getBlock(idx - 1)->next = block;
            }

            newStage->getBlock(newStage->count - 1)->next = nullptr;
        }

        if (list->next) {
            list->next->prev = nullptr;
        }

        descriptor->firstFreeBlock = list->next;

        return list;
    }

    void free(void* addr) {

        if (!addr) {
            return;
        }

        ArenaStage* stage = ArenaMemoryManager::getStage(addr);
        if (stage->large) {
            uint64_t realAddr = uint64_t(addr) - sizeof(ArenaStage);
            freePage((void*) realAddr, stage->count);
            return;
        }

        stage->count++;

        ArenaDescriptor* descriptor = stage->descriptor;
        ArenaBlockNode* block = (ArenaBlockNode*) addr;
        
        if (descriptor->firstFreeBlock) {

            block->next = descriptor->firstFreeBlock;
            descriptor->firstFreeBlock->prev = block;
            descriptor->firstFreeBlock = block;

        } else {

            descriptor->firstFreeBlock = block;
            block->next = nullptr;

        }

        block->prev = nullptr;

        // 暂不考虑释放 arena 页。

    }

    void* allocPage(uint64_t count) {
        uint64_t addr = MemoryManager::getInstance().allocPage(count);
        if (addr) {
        
            addr += MemoryManager::ADDRESS_OF_PHYSICAL_MEMORY_MAP;
            return (void*) addr;

        } else {
        
            return nullptr;
        
        }
    }

    void freePage(void* addr, uint64_t count) {
        uint64_t realAddr = (uint64_t) addr;
        realAddr -= MemoryManager::ADDRESS_OF_PHYSICAL_MEMORY_MAP;
        MemoryManager::getInstance().freePage(realAddr, count);
    }
};
