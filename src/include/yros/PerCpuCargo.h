/*

    每 CPU 数据。

    创建于 2023年2月11日 江西省上饶市玉山县

*/

#pragma once

#include <lib/sys/types.h>

struct PerCpuCargo {
    struct Task* currentTask;
};

extern PerCpuCargo* perCpuCargo;
