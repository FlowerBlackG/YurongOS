// SPDX-License-Identifier: MulanPSL-2.0

/*
 * Differentiated System Description Table
 *
 * 创建于 2023年6月29日 上海市嘉定区安亭镇
 */

#pragma once

#include "./acpi.h"

namespace device {
namespace acpi {

struct DifferentiatedSystemDescriptionTable {
    SystemDescriptorTableHeader sdtHeader;
} __packed;

extern DifferentiatedSystemDescriptionTable* dsdt;

void initDifferentiatedSystemDescriptionTable(DifferentiatedSystemDescriptionTable* table);

}
}
