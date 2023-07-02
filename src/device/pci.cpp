// SPDX-License-Identifier: MulanPSL-2.0

/* 
 * PCI 设备管理
 * 
 * 创建于 2023年6月25日 上海市嘉定区安亭镇
 */


#include "./pci.h"

#include <lib/collections/LinkedList.h>
#include <crt/CRT.h>
#include <lib/stdio.h>
#include <lib/stddef.h>
#include <memory/memory.h>
#include <misc/Kernel.h>

namespace device {
namespace pci {

LinkedList deviceList;
lib::collections::Map<uint16_t, SegmentGroup> segmentGroups;


void enumSegmentGroup(
    uint16_t group,
    intptr_t confSpaceBaseAddr,
    uint8_t startBusNumber,
    uint8_t endBusNumber
) {

    SegmentGroup segmentGroup = {
        .baseAddrOfEnhancedConfMechanism = confSpaceBaseAddr,
        .groupNumber = group,
        .startPCIBusNumber = startBusNumber,
        .endPCIBusNumber = endBusNumber
    };

    segmentGroups[group] = segmentGroup;

    for (int bus = startBusNumber; bus <= endBusNumber; bus++) {
        for (int dev = 0; dev < 32; dev++) {
            checkDevice(segmentGroup, bus, dev);
        }
    }

}

static ExtendedConfigSpace* getExtendedConfigSpace(
    const SegmentGroup& segmentGroup, int32_t bus, int32_t device, int32_t function
) {
    intptr_t addr = segmentGroup.baseAddrOfEnhancedConfMechanism;
    addr += (bus * 256LL + (device * 8) + function) * 4096;
    return (ExtendedConfigSpace*) addr;
}

void checkDevice(
    const SegmentGroup& segmentGroup,
    int32_t bus,
    int32_t device
) {
    for (int function = 0; function < 8; function++) {
        uint32_t value = pci::ioInDWord(bus, device, function, CONF_VENDOR_OFFSET);

        auto configSpace = getExtendedConfigSpace(segmentGroup, bus, device, function);

        uint16_t vendorId = configSpace->vendorId;
        if ( !vendorId || vendorId == 0xffff) {
            // 制造商信息错误。
            return;
        }

        auto deviceStruct = (Device*) memory::kmalloc(sizeof(Device));

        if (!deviceStruct) {
            Kernel::panic("out of memory");
        }

        pci::deviceList.append(&deviceStruct->node);

        deviceStruct->segmentGroup = segmentGroup.groupNumber;
        deviceStruct->bus = bus;
        deviceStruct->device = device;
        deviceStruct->function = function;
        deviceStruct->configSpace = configSpace;


        char s[128];
        sprintf(
            s,
            "(%d, %d, %d): 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n",
            bus, device, function,
            deviceStruct->configSpace->vendorId,
            deviceStruct->configSpace->deviceId,
            deviceStruct->configSpace->revisionId,
            deviceStruct->configSpace->classCode,
            deviceStruct->configSpace->subclass
        );

        CRT::getInstance().write(s);

    }
}

Device* findDeviceById(uint16_t vendorId, uint16_t deviceId) {
    for (auto& it : pci::deviceList) {
        auto device = (Device*) (intptr_t(&it) - offsetof(Device, node));

        if (device->vendorId() == vendorId && device->deviceId() == deviceId) {
            return device;
        }
    }

    return nullptr;
}

}
}
