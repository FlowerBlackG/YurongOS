// SPDX-License-Identifier: MulanPSL-2.0


/* 
 * PCI 设备管理
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

struct Device {
    LinkedListNode node;
    uint8_t bus;
    uint8_t device;
    uint8_t function;
    uint16_t vendorId;
    uint16_t deviceId;
    uint8_t revision;
    uint32_t classCode;
};



void init();
void enumDevice();
void checkDevice(int32_t bus, int32_t device);

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
