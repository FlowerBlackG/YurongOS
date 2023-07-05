// SPDX-License-Identifier: MulanPSL-2.0

/*
 * 内存管理
 *
 * 创建于 2023年7月4日 上海市嘉定区安亭镇
 */

#include "./memory.h"

#include "./MemoryManager.h"

namespace memory {

intptr_t addrOfPhysicalMemoryMap = MemoryManager::ADDRESS_OF_PHYSICAL_MEMORY_MAP;


} // namespace memory


void* operator new (size_t size) {
    return memory::kmalloc(size);
}

void operator delete (void* addr) {
    memory::kfree(addr);
}

