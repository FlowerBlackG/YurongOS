// SPDX-License-Identifier: MulanPSL-2.0

/* 
 * 内存管理
 * 
 * 创建于 2023年6月25日 上海市嘉定区安亭镇
 */


#pragma once


#include "./KernelMemoryAllocator.h"


namespace memory {

/**
 * malloc for the Kernel
 */
inline void* kmalloc(size_t size) {
    return KernelMemoryAllocator::malloc(size);
}

/**
 * free memory for the Kernel 
 */
inline void kfree(void* addr) {
    KernelMemoryAllocator::free(addr);
}


}


