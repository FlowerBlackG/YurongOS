/*

    内核堆内存管理器。

    创建于 2023年2月2日 江西省上饶市玉山县

*/


#pragma once

#include <sys/types.h>


namespace KernelMemoryAllocator {

    void* malloc(uint64_t size);
    void free(void* addr);

    void* allocPage(uint64_t count = 1);
    void freePage(void* addr, uint64_t count = 1);

};
