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

void init() {
    deviceList.clear();
    enumDevice();
}

void enumDevice() {
    for (int bus = 0; bus < 256; bus++) {
        for (int dev = 0; dev < 32; dev++) {
            checkDevice(bus, dev);
        }
    }
}


void checkDevice(int32_t bus, int32_t device) {
    for (int function = 0; function < 8; function++) {
        uint32_t value = pci::ioInDWord(bus, device, function, CONF_VENDOR_OFFSET);

        uint16_t vendorId = value & 0xffff;
        if ( !vendorId || vendorId == 0xffff) {
            // 制造商信息错误。
            return;
        }
        

        auto deviceStruct = (Device*) memory::kmalloc(sizeof(Device));

        if (!deviceStruct) {
            Kernel::panic("out of memory");
        }

        pci::deviceList.append(&deviceStruct->node);

        deviceStruct->bus = bus;
        deviceStruct->device = device;
        deviceStruct->function = function;
        deviceStruct->vendorId = vendorId;
        deviceStruct->deviceId = value >> 16;

        value = pci::ioInDWord(bus, device, function, CONF_COMMAND_OFFSET);
        // todo: command


        value = pci::ioInDWord(bus, device, function, CONF_REVISION_ID_OFFSET);
        deviceStruct->revision = value & 0xff;
        deviceStruct->classCode = value >> 8;



        char s[128];
        sprintf(s, "(%d, %d, %d): 0x%x, 0x%x, 0x%x, 0x%x\n",bus,device,function, vendorId, deviceStruct->deviceId,deviceStruct->revision, deviceStruct->classCode);
        CRT::getInstance().write(s);
        


    }
}

Device* findDeviceById(uint16_t vendorId, uint16_t deviceId) {
    for (auto& it : pci::deviceList) {
        auto device = (Device*) (intptr_t(&it) - offsetof(Device, node));
        if (device->vendorId == vendorId && device->deviceId == deviceId) {
            return device;
        }
    }

    return nullptr;
}

}
}
