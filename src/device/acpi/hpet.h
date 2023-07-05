// SPDX-License-Identifier: MulanPSL-2.0

/*
 * High Precision Event Timer
 *
 * signature "HPET"
 *
 * 创建于 2023年7月5日 上海市嘉定区安亭镇
 */


#pragma once


#include "./acpi.h"


namespace device {
namespace acpi {

struct HighPrecisionEventTimer {
    SystemDescriptorTableHeader sdtHeader;

    uint8_t hardwareRevId;
    uint8_t comparatorCount : 5;
    uint8_t counterSize : 1;
    uint8_t reserved : 1;
    uint8_t legacyReplacement : 1;
    uint16_t pciVendorId;

    struct {
        uint8_t spaceId; // 0 - system memory, 1 - system IO
        uint8_t registerBitWidth;
        uint8_t registerBitOffset;
        uint8_t reserved;
        uint64_t address;
    } __packed address;

    uint8_t hpetNumber;
    uint16_t minimumTick;
    uint8_t pageProtection;
} __packed;

struct HPETGeneralCapabilitiesAndIdRegister {
    const static int regOffset = 0;

    uint64_t revId : 8;
    uint64_t numTimCap : 5;
    uint64_t countSizeCap : 1;
    uint64_t reserved : 1;
    uint64_t legRtCap : 1;
    uint64_t vendorId : 16;
    uint64_t counterClkPeriod : 32;

} __packed;

struct HPETTimerConfigAndCapabilityRegister {
    uint64_t reserved0 : 1;
    uint64_t intTypeCnf : 1;
    uint64_t intEnbCnf : 1;
    uint64_t typeCnf : 1;
    uint64_t perIntCap : 1;
    uint64_t sizeCap : 1;
    uint64_t valSetCnf : 1;
    uint64_t reserved1 : 1;
    uint64_t t32ModeCnf : 1;
    uint64_t intRouteCnf : 5;
    uint64_t fsbEnCnf : 1;
    uint64_t fsbIntDelCap : 1;
    uint64_t reserved2 : 16;
    uint64_t intRouteCap : 32;

    inline HPETTimerConfigAndCapabilityRegister() {
        // nothing to do...
    }

    inline HPETTimerConfigAndCapabilityRegister(int64_t value) {
        *(int64_t*) this = value;
    }

    inline operator int64_t () {
        return *(int64_t*) this;
    }
} __packed;

extern HighPrecisionEventTimer* hpet;

void initHighPrecisionEventTimer(SystemDescriptorTableHeader* table);


}
}
