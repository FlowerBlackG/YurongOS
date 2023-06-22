// SPDX-License-Identifier: MulanPSL-2.0

/*

    每 CPU 数据。

    创建于 2023年2月11日 江西省上饶市玉山县

*/

#pragma once

#include <lib/sys/types.h>

struct PerCpuCargo {

    PerCpuCargo* self;

    /**
     * "卸货甲板"。用于暂存一些物品，如寄存器值。
     */
    char deck[64];
    struct Task* currentTask;
};

extern PerCpuCargo* perCpuCargo;
