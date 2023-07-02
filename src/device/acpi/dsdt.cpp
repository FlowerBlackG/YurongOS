// SPDX-License-Identifier: MulanPSL-2.0

/*
 * Differentiated System Description Table
 *
 * 创建于 2023年6月29日 上海市嘉定区安亭镇
 */

#include "./dsdt.h"

namespace device {
namespace acpi {

DifferentiatedSystemDescriptionTable* dsdt = nullptr;

void initDifferentiatedSystemDescriptionTable(DifferentiatedSystemDescriptionTable* table) {
    acpi::dsdt = reinterpret_cast<decltype(dsdt)>(table);
    if (!dsdt) {
        return;
    }

    // todo: 这里面有设备信息（aml），但我不会用。

}

}
}
