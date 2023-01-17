/*

    内存管理器。

    创建于 2023年1月5日

*/

#include <machine/MemoryManager.h>
#include <CRT.h>
#include <stdio.h>

#include <sys/types.h>
#include <machine/Cmos.h>

void MemoryManager::init() {
    this->processArds();
}

void MemoryManager::processArds() {

    char s[128];

    uint32_t ardsCount = getArdsCount();
    Ards* ardsBuffer = getArdsBuffer();

    sprintf(s, "ards count: %d\n", ardsCount);
    CRT::getInstance().write(s);

    for (int i = 0; i < ardsCount; i++) {
        auto&& ards = ardsBuffer[i];

        sprintf(s, 
            "(base, size, type): (0x%x, 0x%x, %d)\n", 
            (uint32_t) ards.base, 
            (uint32_t) ards.size, 
            ards.type
        );

        CRT::getInstance().write(s);
   
    }

}
