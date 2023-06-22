// SPDX-License-Identifier: MulanPSL-2.0

/*

    stddef header

    创建于 2023年2月12日 江西省上饶市玉山县

*/

#pragma once

#define offsetof(type, member) ((unsigned long) &((type*)0)->member)
