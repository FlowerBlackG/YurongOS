// SPDX-License-Identifier: MulanPSL-2.0

/*
 * Memory Mapped Configuration Space Access Table
 *
 * 创建于 2023年7月2日 上海市嘉定区安亭镇
 */


#include "./mcfg.h"
#include <crt/CRT.h>
#include <lib/stdio.h>
#include <lib/string.h>
#include <device/pci.h>

namespace device {
namespace acpi {


MemoryMappedConfigurationSpaceAccessTable* mcfg = nullptr;

void initMemoryMappedConfigurationSpaceAccessTable(SystemDescriptorTableHeader* table) {
    mcfg = reinterpret_cast<decltype(mcfg)>(table);

    int64_t confSpaceTableCount = mcfg->sdtHeader.length;
    confSpaceTableCount -= sizeof(SystemDescriptorTableHeader) + 8; // 标准头和保留字节。
    confSpaceTableCount /= sizeof(*mcfg->configSpaceBaseAddrAllocation);
    
    char s[128];
    sprintf(s, "  mcfg: there is/are total %lld pcie segment(s)\n", confSpaceTableCount);
    CRT::getInstance().write(s);


    for (int64_t idx = 0; idx < confSpaceTableCount; idx++) {
        auto currConf = mcfg->configSpaceBaseAddrAllocation[idx];
        intptr_t mechanismBaseAddr = currConf.baseAddrOfEnhancedConfMechanism;
        mechanismBaseAddr += memory::MemoryManager::ADDRESS_OF_PHYSICAL_MEMORY_MAP;

        pci::enumSegmentGroup(
            currConf.pciSegmentGroupNumber,
            mechanismBaseAddr,
            currConf.startPCIBusNumber,
            currConf.endPCIBusNumber
        );
    }

}


}
}

