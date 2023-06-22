// SPDX-License-Identifier: MulanPSL-2.0

/*

    每 CPU 数据。

    创建于 2023年2月11日 江西省上饶市玉山县

*/

#include <misc/PerCpuCargo.h>

PerCpuCargo __section(".per_cpu_cargo") cargo[1];

PerCpuCargo* perCpuCargo = cargo;
