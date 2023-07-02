// SPDX-License-Identifier: MulanPSL-2.0

/*
 * Fixed ACPI Description Table
 *
 * 创建于 2023年6月27日 上海市嘉定区安亭镇
 */

#pragma once

#include "./acpi.h"

namespace device {
namespace acpi {



/**
 *
 * https://wiki.osdev.org/FADT
 */
struct FixedACPIDescriptionTable {

    enum class PreferredPowerManagementProfile : uint8_t {
        UNSPECIFIED = 0,
        DESKTOP = 1,
        MOBILE = 2,
        WORKSTATION = 3,
        ENTERPRISE_SERVER = 4,
        SOHO_SERVER = 5,
        APLLIANCE_PC = 6,
        PERFORMANCE_SERVER = 7,

    };

    struct GenericAddress {
        uint8_t addressSpace;
        uint8_t bitWidth;
        uint8_t bitOffset;
        uint8_t accessSize;
        uint64_t address;
    };

    SystemDescriptorTableHeader sdtHeader;
    uint32_t firmwareCtrl;
    uint32_t dsdt;

    uint8_t reserved;

    PreferredPowerManagementProfile preferredPowerManagementProfile;
    uint16_t sciInterrupt;
    uint32_t smiCommandPort;
    uint8_t acpiEnable;
    uint8_t acpiDisable;
    uint8_t s4BiosReq;

    uint8_t  pStateControl;
    uint32_t pnm1aEventBlock;
    uint32_t pm1bEventBlock;
    uint32_t pm1aControlBlock;
    uint32_t pm1bControlBlock;
    uint32_t pm2ControlBlock;
    uint32_t pmTimerBlock;
    uint32_t gpe0Block;
    uint32_t gpe1Block;
    uint8_t  pm1EventLength;
    uint8_t  pm1ControlLength;
    uint8_t  pm2ControlLength;
    uint8_t  pmTimerLength;
    uint8_t  gpe0Length;
    uint8_t  gpe1Length;
    uint8_t  gpe1Base;
    uint8_t  cStateControl;
    uint16_t worstC2Latency;
    uint16_t worstC3Latency;
    uint16_t flushSize;
    uint16_t flushStride;
    uint8_t  dutyOffset;
    uint8_t  dutyWidth;
    uint8_t  dayAlarm;
    uint8_t  monthAlarm;
    uint8_t  century;

    // reserved in ACPI 1.0; used since ACPI 2.0+
    uint16_t bootArchitectureFlags;

    uint8_t  reserved2;
    uint32_t flags;


    GenericAddress resetReg;

    uint8_t  resetValue;
    uint8_t  reserved3[3];

    // 64bit pointers - Available on ACPI 2.0+
    uint64_t                xFirmwareControl;
    uint64_t                xDsdt;

    GenericAddress xPM1aEventBlock;
    GenericAddress xPM1bEventBlock;
    GenericAddress xPM1aControlBlock;
    GenericAddress xPM1bControlBlock;
    GenericAddress xPM2ControlBlock;
    GenericAddress xPMTimerBlock;
    GenericAddress xGPE0Block;
    GenericAddress xGPE1Block;

} __packed;



extern FixedACPIDescriptionTable* fadt;

inline bool fadtUnder4GB() {
    return fadt == 0 ||
        intptr_t(fadt) - memory::MemoryManager::ADDRESS_OF_PHYSICAL_MEMORY_MAP < 4LL * 1024 * 1024 * 1024;
}

void initFixedACPIDescriptionTable(SystemDescriptorTableHeader* table);


}
}
