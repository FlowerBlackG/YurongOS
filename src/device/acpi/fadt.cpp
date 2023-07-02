// SPDX-License-Identifier: MulanPSL-2.0

/*
 * Fixed ACPI Description Table
 *
 * 创建于 2023年6月27日 上海市嘉定区安亭镇
 */

#include "./fadt.h"
#include "./facs.h"
#include "./dsdt.h"
#include <misc/Kernel.h>

namespace device {
namespace acpi {

FixedACPIDescriptionTable* fadt = nullptr;

void initFixedACPIDescriptionTable(SystemDescriptorTableHeader* table) {
    fadt = reinterpret_cast<decltype(fadt)>(table);
    if (!fadt) {
        return;
    }

    if (!fadtUnder4GB()) {
        Kernel::panic("[not impl] acpi: fadt is not under 4GB, which is not supported yet.\n");
    }

    // firmware ctrl
    intptr_t fc = fadt->firmwareCtrl;
    if (fc) {
        fc += memory::MemoryManager::ADDRESS_OF_PHYSICAL_MEMORY_MAP;
        acpi::initFirmwareACPIControlStructure(reinterpret_cast<SystemDescriptorTableHeader*>(fc));
    }

    // dsdt
    intptr_t dsdt = fadtUnder4GB() ? fadt->dsdt : fadt->xDsdt;
    dsdt += memory::MemoryManager::ADDRESS_OF_PHYSICAL_MEMORY_MAP;
    initDifferentiatedSystemDescriptionTable(reinterpret_cast<DifferentiatedSystemDescriptionTable*>(dsdt));


}

} // namespace acpi
} // namespace device