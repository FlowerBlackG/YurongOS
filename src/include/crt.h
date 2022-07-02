/*
 * CRT 显示器控制头文件。
 * 创建于 2022年7月2日。
 */

#pragma once

#include <sys/types.h>

#ifndef CRT_ADDR_REG
    #define CRT_ADDR_REG 0x3d4
#endif

#ifndef CRT_DATA_REG
    #define CRT_DATA_REG 0x3d5
#endif

#ifndef CRT_REG_CURSOR_HIGH
    #define CRT_REG_CURSOR_HIGH 0xe
#endif

#ifndef CRT_REG_CURSOR_LOW
    #define CRT_REG_CURSOR_LOW 0xf
#endif
