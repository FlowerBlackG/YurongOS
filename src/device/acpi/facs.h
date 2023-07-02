// SPDX-License-Identifier: MulanPSL-2.0

/*
 * Firmware ACPI Control Structure
 *
 * 创建于 2023年6月29日 上海市嘉定区安亭镇
 */

#pragma once

#include "./acpi.h"

namespace device {
namespace acpi {

struct FirmwareACPIControlStructure  {

    SystemDescriptorTableHeader sdtHeader;


} __packed;

extern FirmwareACPIControlStructure* facs;

void initFirmwareACPIControlStructure(SystemDescriptorTableHeader* table);

}
}
