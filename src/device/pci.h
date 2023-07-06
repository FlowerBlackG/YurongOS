// SPDX-License-Identifier: MulanPSL-2.0


/* 
 * PCI 设备管理
 * 使用 PCI Express 方式。不使用传统方式。
 *
 * 创建于 2023年6月25日 上海市嘉定区安亭镇
 */


/*
 * PCI 通用头
 * 
 * reg  off  bits 31-24 bits 23-16   bits 15-8    bits 7-0
 * ------------------------------------------------------------
 * 0x0  0x0 |       device id      |        vendor id
 * 0x1  0x4 |       status         |        command
 * 0x2  0x8 |class code|sub class  | prog if     |revision id
 * 0x3  0xC | BIST     |header type|latency timer|cacheline size
 * 
 */

#pragma once

#include <lib/sys/types.h>
#include <lib/collections/LinkedList.h>
#include <lib/collections/Map.hpp>
#include <misc/io.h>

namespace device {
namespace pci {

const int CONF_ADDR_REG = 0xCF8;
const int CONF_DATA_REG = 0xCFC;

const int CONF_VENDOR_OFFSET = 0x0;
const int CONF_DEVICE_ID_OFFSET = 0x2;
const int CONF_COMMAND_OFFSET = 0x4;
const int CONF_STATUS_OFFSET = 0x6;
const int CONF_REVISION_ID_OFFSET = 0x8;
const int CONF_PROG_IF_OFFSET = 0x9;
const int CONF_SUB_CLASS_OFFSET = 0xa;
const int CONF_CLASS_CODE_OFFSET = 0xb;
const int CONF_CACHE_LINE_SIZE_OFFSET = 0xc;
const int CONF_LATENCY_TIMER_OFFSET = 0xd;
const int CONF_HEADER_TYPE_OFFSET = 0xe;
const int CONF_BIST_OFFSET = 0xf;


struct ConfigAddress {
    uint32_t registerOffset : 8;
    uint32_t function : 3;
    uint32_t device : 5;
    uint32_t bus : 8;
    uint32_t reserved : 7;
    uint32_t enable : 1;

    operator int32_t() {
        return *(int32_t*) this;
    }

} __packed;

struct Capability {
    int8_t capabilityId;
    uint8_t nextCapabilityPtr;
    uint16_t messageControl;
    uint32_t messageAddress;
    uint16_t messageData;
};

struct SegmentGroup {
    intptr_t baseAddrOfEnhancedConfMechanism;
    uint16_t groupNumber;
    uint8_t startPCIBusNumber;
    uint8_t endPCIBusNumber;
};

extern lib::collections::Map<uint16_t, SegmentGroup> segmentGroups;


struct ExtendedConfigSpace {
    uint16_t vendorId;
    uint16_t deviceId;

    uint16_t command;
    uint16_t status;

    uint8_t revisionId;
    uint8_t progIf;
    uint8_t subclass;
    uint8_t classCode;

    uint8_t cacheLineSize;

    // 0x0D
    uint8_t latencyTimer;

    // 0x0E
    uint8_t headerType : 7;
    // if MF = 1, then this device has multiple functions.
    uint8_t headerTypeMF : 1;

    // 0x0F
    uint8_t bist;

} __packed;

struct BaseAddressRegister {
    uint32_t value;

    inline bool isMemorySpaceBAR() {
        return !(value & 1);
    }

    inline bool isIOSpaceBAR() {
        return (value & 1);
    }

    inline uint32_t baseAddress() {
        if (isIOSpaceBAR()) {
            return value >> 2;
        } else {
            return value >> 4;
        }
    }

    inline bool prefetchable() {
        return value & 0x8;
    }

    inline uint8_t memorySpaceBARType() {
        return (value >> 1) & 0x3;
    }
} __packed;

struct ExtendedConfigSpaceType0 : ExtendedConfigSpace {

    // common header derived from parent...

    BaseAddressRegister bar[6];

    uint32_t cardbusCISPointer;

    uint16_t subsystemVendorId;
    uint16_t subsystemId;

    uint32_t expansionROMBaseAddress;

    uint32_t capabilitiesPointer : 8;
    uint32_t reserved0 : 24;

    uint32_t reserved1 : 32;

    uint8_t interruptLine;
    uint8_t interruptPin;
    uint8_t minGrant;
    uint8_t maxLatency;

} __packed;

/**
 * PCI 设备结构。
 */
struct Device {
    LinkedListNode node;
    uint16_t segmentGroup;
    uint8_t bus;
    uint8_t device;
    uint8_t function;
    ExtendedConfigSpace* configSpace;

    inline uint16_t deviceId() {
        return configSpace->deviceId;
    }

    inline uint16_t vendorId() {
        return configSpace->vendorId;
    }
};

void enumSegmentGroup(
    uint16_t group, intptr_t confSpaceBaseAddr,
    uint8_t startBusNumber, uint8_t endBusNumber
);

void checkDevice(
    const SegmentGroup& segmentGroup,
    int32_t bus,
    int32_t device
);

Device* findDeviceById(uint16_t vendorId, uint16_t deviceId);

inline ConfigAddress formConfigAddress(
    uint32_t bus,
    uint32_t device,
    uint32_t function,
    uint32_t address
) {
    uint32_t res = 0x80000000; // enable
    res |= address & 0xff;
    res |= (function & 0x7) << 8;
    res |= (device & 0x1f) << 11;
    res |= (bus & 0xff) << 16;


    return *(ConfigAddress*) &res;
}


void inline ioOutDWord(
    uint8_t bus, uint8_t dev, uint8_t func,
    uint8_t addr, uint32_t value
) {
    io::outDWord(CONF_ADDR_REG, formConfigAddress(bus, dev, func, addr));
    io::outDWord(CONF_DATA_REG, value);
}

uint32_t inline ioInDWord(
    uint8_t bus, uint8_t dev, uint8_t func, 
    uint8_t addr
) {
    io::outDWord(CONF_ADDR_REG, formConfigAddress(bus, dev, func, addr));
    
    return io::inDWord(CONF_DATA_REG);
}



#if 0
static void __check_size() {
    sizeof(ConfigAddress);
}
#endif

}
}
