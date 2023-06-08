/*

    内存管理器。

    创建于 2023年1月5日

*/

#include <memory/MemoryManager.h>
#include <memory/ArenaMemoryManager.h>
#include <crt/CRT.h>
#include <lib/stdio.h>
#include <lib/string.h>

#include <lib/sys/types.h>
#include <machine/Cmos.h>

namespace MemoryManager {

    uint64_t systemTotalMemory;
    uint64_t systemManagedMemory;
    uint64_t systemManagedMildMemory;
}

void MemoryManager::init() {
    
    FreeMemoryManager::init();

    systemTotalMemory = 0;
    systemManagedMemory = 0;
    systemManagedMildMemory = 0;

    processArds();

    ArenaMemoryManager::init();
}

void MemoryManager::processArds() {

    char s[512];

    uint32_t ardsCount = getArdsCount();
    Ards* ardsBuffer = getArdsBuffer();

    sprintf(s, "ards count: %d\n", ardsCount);
    CRT::getInstance().write(s);

    if (ardsCount > 33) {
        ardsCount = 33;
        CRT::getInstance().write("too much ards! limit to 33.\n");
    }

    for (uint32_t i = 0; i < ardsCount; i++) {
        auto&& ards = ardsBuffer[i];

        auto base = ards.base;
        auto size = ards.size;

        // 输出这段内存的信息。
        sprintf(s, 
            "(base, size, type): (0x%llx, 0x%llx, %d) %s\n", 
            base, 
            size, 
            ards.type,
            (ards.type == 1 ? "ok" : "restricted")
        );

        CRT::getInstance().write(s);

        systemTotalMemory += size;

        if (ards.type != 1) {
            continue; // 该段内存不允许使用。
        }

        systemManagedMemory += size;

        if (base + size < BEGIN_OF_USABLE_ADDRESS) {
            continue; // 内存位于不好用的区域。
        }


        // 执行到这里，说明这段内存可能可用。
        if (base < BEGIN_OF_USABLE_ADDRESS) {
            size -= BEGIN_OF_USABLE_ADDRESS - base;
            base = BEGIN_OF_USABLE_ADDRESS;
        }

        // 基地址 4KB 对齐。
        if (base % 4096 != 0) {
            auto prevBase = base;
            base /= 4096;
            base *= 4096;
            base += 4096;
            
            if (base - prevBase >= size) {
                continue; // 对齐后没有剩余空间。
            }

            size -= base - prevBase;
        }

        // 令尺寸 4KB 对齐。
        size /= 4096;
        size *= 4096;

        if (size == 0) {
            continue; // 无法使用这段空间。
        }

        systemManagedMildMemory += size;

        // 登记到管理器。
        FreeMemoryManager::free(base, size);
    }

    sprintf(
        s, 
        "[info] system total memory: %dG %dM %dK %dB\n"
        "[info] kernel managed     : %dG %dM %dK %dB\n"
        "[info] kernel mild memory : %dG %dM %dK %dB\n"
        "[info] freeMemMgr managed : %dG %dM %dK %dB\n",

        systemTotalMemory >> 30, 
        (systemTotalMemory >> 20) % 1024,
        (systemTotalMemory >> 10) % 1024,
        systemTotalMemory % 1024,

        systemManagedMemory >> 30, 
        (systemManagedMemory >> 20) % 1024,
        (systemManagedMemory >> 10) % 1024,
        systemManagedMemory % 1024,

        systemManagedMildMemory >> 30, 
        (systemManagedMildMemory >> 20) % 1024,
        (systemManagedMildMemory >> 10) % 1024,
        systemManagedMildMemory % 1024,

        FreeMemoryManager::getTotalFreeMemory() >> 30, 
        (FreeMemoryManager::getTotalFreeMemory() >> 20) % 1024,
        (FreeMemoryManager::getTotalFreeMemory() >> 10) % 1024,
        FreeMemoryManager::getTotalFreeMemory() % 1024
    );

    CRT::getInstance().write(s);

    if (systemManagedMildMemory != FreeMemoryManager::getTotalFreeMemory()) {
        CRT::getInstance().write(
            "[warning] mild memory doesn't match free memory manager managed memory.\n"
        );

    }

}

uint64_t MemoryManager::allocPage(uint64_t count) {
    return FreeMemoryManager::alloc(MemoryManager::PAGE_SIZE * count);
}

uint64_t MemoryManager::allocWhitePage(uint64_t count, uint8_t fill) {
    uintptr_t resAddr = FreeMemoryManager::alloc(MemoryManager::PAGE_SIZE * count);

    if (resAddr) {
        memset((void*) (resAddr + ADDRESS_OF_PHYSICAL_MEMORY_MAP), fill, PAGE_SIZE);
    }

    return resAddr;
}

void MemoryManager::freePage(uint64_t addr, uint64_t count) {
    FreeMemoryManager::free(addr, MemoryManager::PAGE_SIZE * count);
}

int MemoryManager::mapPage(
    PageMapLevel4 pml4, 
    uintptr_t linearAddress, 
    uintptr_t pagePhysicalAddress,
    bool rw,
    bool us
) {

    int pml1idx = (linearAddress >> 12) % 512;
    int pml2idx = (linearAddress >> 21) % 512;
    int pml3idx = (linearAddress >> 30) % 512;
    int pml4idx = (linearAddress >> 39) % 512;

    PageMapLevel4Entry& pml4e = pml4[pml4idx];

    uintptr_t pml3addr = pml4e.pageFrameNumber * PAGE_SIZE;
    PageMapLevel3 pml3;

    if ( !pml3addr ) {
        pml3addr = allocWhitePage();
        
        if (!pml3addr) {
            return 1;
        }

        pml4e.pageFrameNumber = pml3addr / PAGE_SIZE;
        pml4e.rw = !!rw;
        pml4e.us = !!us;
        pml4e.present = 1;
    }

    pml3 = (PageMapLevel3) (pml3addr + ADDRESS_OF_PHYSICAL_MEMORY_MAP);
    PageMapLevel3Entry& pml3e = pml3[pml3idx];

    uintptr_t pml2addr = pml3e.pageFrameNumber * PAGE_SIZE;
    PageMapLevel2 pml2;

    if ( !pml2addr ) {
        pml2addr = allocWhitePage();

        if (!pml2addr) {
            freePage(pml3addr);
            pml4e.pageFrameNumber = 0;
            return 2;
        }

        pml3e.pageFrameNumber = pml2addr / PAGE_SIZE;
        pml3e.rw = !!rw;
        pml3e.us = !!us;
        pml3e.present = 1;
    }

    pml2 = (PageMapLevel2) (pml2addr + ADDRESS_OF_PHYSICAL_MEMORY_MAP);
    PageMapLevel2Entry& pml2e = pml2[pml2idx];

    uintptr_t pml1addr = pml2e.pageFrameNumber * PAGE_SIZE;
    PageMapLevel1 pml1;

    if ( !pml1addr ) {
        pml1addr = allocWhitePage();

        if (!pml1addr) {
            freePage(pml3addr);
            freePage(pml2addr);
            pml4e.pageFrameNumber = 0;
            return 2;
        }

        pml2e.pageFrameNumber = pml1addr / PAGE_SIZE;
        pml2e.rw = !!rw;
        pml2e.us = !!us;
        pml2e.present = 1;
    }

    pml1 = (PageMapLevel1) (pml1addr + ADDRESS_OF_PHYSICAL_MEMORY_MAP);
    PageMapLevel1Entry& pml1e = pml1[pml1idx];

    pml1e.rw = !!rw;
    pml1e.us = !!us;
    pml1e.present = 1;
    pml1e.pageFrameNumber = pagePhysicalAddress / PAGE_SIZE;

    return 0;
}


void MemoryManager::walkPageTables(
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
) {

    const int pageSize = MemoryManager::PAGE_SIZE;

    const int pml4EntriesPerPage = pageSize / sizeof(PageMapLevel4Entry);
    const int pml3EntriesPerPage = pageSize / sizeof(PageMapLevel3Entry);
    const int pml2EntriesPerPage = pageSize / sizeof(PageMapLevel2Entry);
    const int pml1EntriesPerPage = pageSize / sizeof(PageMapLevel1Entry);

    const auto&& walkPml1 = [
        &onPml1EntryDiscovered = onPml1EntryDiscovered,
        &cargo = cargo
    ] (
        PageMapLevel4 pml4, PageMapLevel4Entry& pml4e,
        PageMapLevel3 pml3, PageMapLevel3Entry& pml3e,
        PageMapLevel2 pml2, PageMapLevel2Entry& pml2e,
        PageMapLevel1 pml1
    ) {

        for (int idx = 0; idx < pml1EntriesPerPage; idx++) {
        
           
            PageMapLevel1Entry& entry = pml1[idx];
            auto cmd = onPml1EntryDiscovered(
                cargo, pml4, pml4e, pml3, pml3e, pml2, pml2e, pml1, entry
            );

            if (cmd == WalkPageTablesCommand::WALK_INTO) {
                // do nothing
            } else if (cmd == WalkPageTablesCommand::BACK_TO_UPPER) {
                break;
            } else if (cmd == WalkPageTablesCommand::SKIP_THIS_ENTRY) {
                // do nothing
            } else {
                // 不应该抵达这里。
            }

            

        }
    };
    
    const auto&& walkPml2 = [
        &onPml2EntryDiscovered = onPml2EntryDiscovered,
        &walkPml1 = walkPml1,
        &cargo = cargo
    ] (
        PageMapLevel4 pml4, PageMapLevel4Entry& pml4e,
        PageMapLevel3 pml3, PageMapLevel3Entry& pml3e,
        PageMapLevel2 pml2
    ) {
        for (int idx = 0; idx < pml2EntriesPerPage; idx++) {
        
            PageMapLevel2Entry& entry = pml2[idx];
            auto cmd = onPml2EntryDiscovered(cargo, pml4, pml4e, pml3, pml3e, pml2, entry);

            if (cmd == WalkPageTablesCommand::WALK_INTO) {
                uintptr_t pml1addr = entry.pageFrameNumber * PAGE_SIZE;
                PageMapLevel1 pml1 = (PageMapLevel1) (pml1addr + ADDRESS_OF_PHYSICAL_MEMORY_MAP);
                walkPml1(pml4, pml4e, pml3, pml3e, pml2, entry, pml1);
            } else if (cmd == WalkPageTablesCommand::BACK_TO_UPPER) {
                break;
            } else if (cmd == WalkPageTablesCommand::SKIP_THIS_ENTRY) {
                // do nothing
            } else {
                // 不应该抵达这里。
            }

        }
    };

    const auto&& walkPml3 = [
        &onPml3EntryDiscovered = onPml3EntryDiscovered,
        &walkPml2 = walkPml2,
        &cargo = cargo
    ] (
        PageMapLevel4 pml4, PageMapLevel4Entry& pml4e,
        PageMapLevel3 pml3
    ) {

        for (int idx = 0; idx < pml3EntriesPerPage; idx++) {
            PageMapLevel3Entry& entry = pml3[idx];
            auto cmd = onPml3EntryDiscovered(cargo, pml4, pml4e, pml3, entry);

            if (cmd == WalkPageTablesCommand::WALK_INTO) {
                uintptr_t pml2addr = entry.pageFrameNumber * PAGE_SIZE;
                PageMapLevel2 pml2 = (PageMapLevel2) (pml2addr + ADDRESS_OF_PHYSICAL_MEMORY_MAP);
                walkPml2(pml4, pml4e, pml3, entry, pml2);
            } else if (cmd == WalkPageTablesCommand::BACK_TO_UPPER) {
                break;
            } else if (cmd == WalkPageTablesCommand::SKIP_THIS_ENTRY) {
                // do nothing
            } else {
                // 不应该抵达这里。
            }
        }

    };


    // walk pml 4
    for (int idx = 0; idx < pml4EntriesPerPage; idx++) {
        PageMapLevel4Entry& entry = pml4[idx];
        auto cmd = onPml4EntryDiscovered(cargo, pml4, entry);

        if (cmd == WalkPageTablesCommand::WALK_INTO) {
            uintptr_t pml3addr = entry.pageFrameNumber * PAGE_SIZE;
            PageMapLevel3 pml3 = (PageMapLevel3) (pml3addr + ADDRESS_OF_PHYSICAL_MEMORY_MAP);
            walkPml3(pml4, entry, pml3);
        } else if (cmd == WalkPageTablesCommand::BACK_TO_UPPER) {
            break;
        } else if (cmd == WalkPageTablesCommand::SKIP_THIS_ENTRY) {
            // do nothing
        } else {
            // 不应该抵达这里。
        }

        

    }

}
