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

        sprintf(s, 
            "(base, size, type): (0x%llx, 0x%llx, %d) %s\n", 
            ards.base, 
            ards.size, 
            ards.type,
            (ards.type == 1 ? "ok" : "denied")
        );

        CRT::getInstance().write(s);
   
    }

}
