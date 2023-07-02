// SPDX-License-Identifier: MulanPSL-2.0

/*
 * Advanced Configuratio and Power Interface
 *
 * 创建于 2023年6月25日 上海市嘉定区安亭镇
 */

#pragma once

#include <lib/sys/types.h>
#include <memory/MemoryManager.h>

namespace device {
namespace acpi {

struct RootSystemDescriptor {

    /**
     * "RSD PTR "
     */
    int8_t signature[8];
    uint8_t checksum;
    int8_t oemId[6];

    /**
     * 版本号。0 为 1.0 版本。1 为后续版本。
     * 只有当版本号为1时，用且必须用 XSDT。
     */
    uint8_t revision;

    /**
     * R(Root)SDT 的地址。物理地址。
     */
    uint32_t rsdtAddress;
} __packed;

struct RootSystemDescriptor2 : RootSystemDescriptor {
    // RootSystemDescriptor

    // extended area
    uint32_t length;
    uint64_t xsdtAddress;
    uint8_t extendedChecksum;
    uint8_t reserved[3];

} __packed;

struct SystemDescriptorTableHeader {
    int8_t signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    int8_t oemId[6];
    int8_t oemTableId[8];
    uint32_t oemRevision;
    uint32_t creatorId;
    uint32_t creatorRevision;
} __packed;

struct RootSystemDescriptionTable {

    SystemDescriptorTableHeader sdtHeader;

    // extra data
    inline SystemDescriptorTableHeader* getSDT(int index) {
        if (index < 0 || index >= (this->sdtHeader.length - sizeof(SystemDescriptorTableHeader)) / 4) {
            return nullptr;
        }

        auto phyAddr = * (uint32_t*) (index * 4 + intptr_t(this) + sizeof(SystemDescriptorTableHeader));
        auto linearAddr = phyAddr + memory::MemoryManager::ADDRESS_OF_PHYSICAL_MEMORY_MAP;

        return reinterpret_cast<SystemDescriptorTableHeader*>(linearAddr);
    }
} __packed;

struct ExtendedRootSystemDescriptorTable {

    SystemDescriptorTableHeader sdtHeader;

    inline SystemDescriptorTableHeader* getSDT(int index) {
        if (index < 0 || index >= (this->sdtHeader.length - sizeof(SystemDescriptorTableHeader)) / 8) {
            return nullptr;
        }

        auto phyAddr = * (uint64_t*) (index * 8 + intptr_t(this) + sizeof(SystemDescriptorTableHeader));
        auto linearAddr = phyAddr + memory::MemoryManager::ADDRESS_OF_PHYSICAL_MEMORY_MAP;

        return reinterpret_cast<SystemDescriptorTableHeader*>(linearAddr);
    }

} __packed;

extern RootSystemDescriptor* rootSystemDescriptor;
extern RootSystemDescriptor2* rootSystemDescriptor2;
extern RootSystemDescriptionTable* rootSystemDescriptionTable;
extern ExtendedRootSystemDescriptorTable* extendedRootSystemDescriptorTable;

void init();





}
}
