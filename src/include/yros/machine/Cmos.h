/*


    创建时间：2023年1月5日

*/

#pragma once

#include <lib/sys/types.h>

namespace Cmos {

    const uint16_t CMOS_ADDR_PORT = 0x70;
    const uint16_t CMOS_DATA_PORT = 0x71;

    const uint8_t OFFSET_BASE_MEMORY_SIZE_LOW = 0x15;
    const uint8_t OFFSET_BASE_MEMORY_SIZE_HIGH = 0x16;
    const uint8_t OFFSET_EXTENDED_MEMORY_SIZE_LOW = 0x17;
    const uint8_t OFFSET_EXTENDED_MEMORY_SIZE_HIGH = 0x18;
    const uint8_t OFFSET_MEMORY_SIZE_ABOVE_1MB_LOW = 0x30;
    const uint8_t OFFSET_MEMORY_SIZE_ABOVE_1MB_HIGH = 0x31;
    

    uint8_t readByte(uint8_t offset);
}
