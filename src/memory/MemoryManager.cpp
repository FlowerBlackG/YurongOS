/*

    内存管理器。

    创建于 2023年1月5日

*/

#include <memory/MemoryManager.h>
#include <CRT.h>
#include <stdio.h>

#include <sys/types.h>
#include <machine/Cmos.h>

void MemoryManager::init() {
    this->freeMemoryManager.init();

    this->systemTotalMemory = 0;
    this->systemManagedMemory = 0;
    this->systemManagedMildMemory = 0;

    this->processArds();
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

        this->systemTotalMemory += size;

        if (ards.type != 1) {
            continue; // 该段内存不允许使用。
        }

        this->systemManagedMemory += size;

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

        this->systemManagedMildMemory += size;

        // 登记到管理器。
        freeMemoryManager.free(base, size);
    }

    sprintf(
        s, 
        "[info] system total memory: %dG %dM %dK %dB\n"
        "[info] system managed     : %dG %dM %dK %dB\n"
        "[info] system mild memory : %dG %dM %dK %dB\n"
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

        freeMemoryManager.getTotalFreeMemory() >> 30, 
        (freeMemoryManager.getTotalFreeMemory() >> 20) % 1024,
        (freeMemoryManager.getTotalFreeMemory() >> 10) % 1024,
        freeMemoryManager.getTotalFreeMemory() % 1024
    );

    CRT::getInstance().write(s);

    if (this->systemManagedMildMemory != freeMemoryManager.getTotalFreeMemory()) {
        CRT::getInstance().write(
            "[warning] mild memory doesn't match free memory manager managed memory.\n"
        );

    }

}
