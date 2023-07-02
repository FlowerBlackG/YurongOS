// SPDX-License-Identifier: MulanPSL-2.0

/*
 * Memory Mapped Configuration Space Access Table
 *
 * 创建于 2023年7月2日 上海市嘉定区安亭镇
 */

#pragma once

#include "./acpi.h"

namespace device {
namespace acpi {

struct MemoryMappedConfigurationSpaceAccessTable {
    SystemDescriptorTableHeader sdtHeader;
    int64_t reserved;

    /**
     * Configuration Space Base Address Allocation Structures
     * https://wiki.osdev.org/PCI_Express#Enhanced_Configuration_Mechanism
     */
    struct ConfigSpaceBaseAddrAllocation {
        int64_t baseAddrOfEnhancedConfMechanism;
        int16_t pciSegmentGroupNumber;
        uint8_t startPCIBusNumber;
        uint8_t endPCIBusNumber;
        int32_t reserved;
    } __packed configSpaceBaseAddrAllocation[0];

} __packed;

extern MemoryMappedConfigurationSpaceAccessTable* mcfg;

void initMemoryMappedConfigurationSpaceAccessTable(SystemDescriptorTableHeader* table);



}
}
