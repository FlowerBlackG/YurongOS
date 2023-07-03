// SPDX-License-Identifier: MulanPSL-2.0

/*

    串口驱动。
    创建于 2023年6月8日 上海市嘉定区安亭镇

*/

#pragma once

#include <lib/sys/types.h>

namespace Serial {

const int16_t COM1_IO_PORT_BASE = 0x3F8;
const int16_t COM2_IO_PORT_BASE = 0x2F8;

const int16_t COM_IO_DATA_OFFSET = 0;
const int16_t COM_IO_INTERRUPT_ENABLE_REG_OFFSET = 1;

// todo

//int write()


}
