// SPDX-License-Identifier: MulanPSL-2.0

/*
 * Advanced Configuratio and Power Interface
 *
 * 创建于 2023年6月25日 上海市嘉定区安亭镇
 */

#include "./acpi.h"

#include <memory/MemoryManager.h>
#include <lib/stdio.h>
#include <lib/string.h>
#include <crt/CRT.h>
#include <misc/Kernel.h>

namespace device {
namespace acpi {

RootSystemDescriptor* rootSystemDescriptor = nullptr;
RootSystemDescriptor2* rootSystemDescriptor2 = nullptr;

static inline intptr_t addrOfPhysicalMemoryMap() {
    return memory::MemoryManager::ADDRESS_OF_PHYSICAL_MEMORY_MAP;
}

static inline bool findRsdt() {

    const char* flag = "RSD PTR ";
    const int64_t iFlag = *reinterpret_cast<const int64_t*>(flag);

    const auto search = [&] (intptr_t start, intptr_t end) {

        for (intptr_t curr = start; curr < end; curr += 16) {
            auto sign = *reinterpret_cast<int64_t*>(curr);
            if (sign == iFlag) {
                // found!! 好耶！！
                return curr;
            }
        }

        return intptr_t(0);
    };

    intptr_t xsdtAddr;

    // first, let's search in Extended BIOS Data Area

    auto ppEBDA = addrOfPhysicalMemoryMap() + 0x40e;

    auto pEBDA = *reinterpret_cast<int16_t*>(ppEBDA);
    // pEBDA 是物理地址。不能直接访问。要转换成线性地址。
    auto pEDALinear = pEBDA + addrOfPhysicalMemoryMap();
    // 在 1KB 范围里搜索。
    xsdtAddr = search(pEDALinear, pEDALinear + 1024);

    // second, search it from main BIOS area
    if (!xsdtAddr) {
        xsdtAddr = search(
            0xe0000 + addrOfPhysicalMemoryMap(),
            0xfffff + addrOfPhysicalMemoryMap()
        );
    }

    if (!xsdtAddr) {
        return false;
    }

    acpi::rootSystemDescriptor = reinterpret_cast<RootSystemDescriptor*>(xsdtAddr);
    if (acpi::rootSystemDescriptor->revision > 0) {
        acpi::rootSystemDescriptor2 = reinterpret_cast<RootSystemDescriptor2*>(xsdtAddr);
    }

    char oemId[7];
    strncpy(oemId, acpi::rootSystemDescriptor->oemId, 6);
    oemId[6] = '\0';

    char s[32];
    sprintf(s, "[info] acpi: OEM is [%s]\n", oemId);
    CRT::getInstance().write(s);

    return true;
}

void init() {
    if (!findRsdt()) {
        Kernel::panic("failed to find xsdt.");
    }
}

}
}
