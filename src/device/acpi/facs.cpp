// SPDX-License-Identifier: MulanPSL-2.0

/*
 * Firmware ACPI Control Structure
 *
 * 创建于 2023年6月29日 上海市嘉定区安亭镇
 */



#include "./facs.h"

namespace device {
namespace acpi {

FirmwareACPIControlStructure* facs = nullptr;

void initFirmwareACPIControlStructure(SystemDescriptorTableHeader* table) {
    facs = reinterpret_cast<decltype(facs)>(table);
    if (!facs) {
        return;
    }

    // todo: 暂时不知道该做什么。这张表似乎是空的？
}


}
}
