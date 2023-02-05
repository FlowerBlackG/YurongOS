/*

    中断处理通用入口和简单处理器。

    创建于2023年2月1日 江西省上饶市玉山县

*/

#pragma once

#include <yros/machine/X86Assembly.h>

#define IMPLEMENT_EXCEPTION_ENTRANCE(entranceFunctionName, handlerName) \
    void entranceFunctionName() { \
        x86asmSaveContext(); \
        __asm ("lea -0x8(%rsi), %rsi"); \
        \
        x86asmDirectCall(handlerName); \
        x86asmRestoreContext(); \
        \
        x86asmLeave(); \
        x86asmIret(); \
    }

#define IMPLEMENT_EXCEPTION_WITH_ERRCODE_ENTRANCE(entranceFunctionName, handlerName) \
    void entranceFunctionName() { \
        x86asmSaveContext(); \
        \
        x86asmDirectCall(handlerName); \
        x86asmRestoreContext(); \
        x86asmLeave(); \
        __asm ("addq $8, %rsp"); \
        x86asmIret(); \
    }

#define IMPLEMENT_EXCEPTION_HANDLER(handlerName, errorMsg, signalValue) \
    void handlerName( \
        SoftwareContextRegisters* softwareRegs, \
        HardwareContextRegisters* hardwareRegs \
    ) { \
        CRT::getInstance().write(errorMsg); \
        CRT::getInstance().write("\n"); \
        x86asmCli(); \
        while (1) \
            ; \
    }

#define IMPLEMENT_EXCEPTION_WITH_ERRCODE_HANDLER(handlerName, errorMsg, signalValue) \
    void handlerName( \
        SoftwareContextRegisters* softwareRegs, \
        HardwareContextRegisters* hardwareRegs \
    ) { \
        CRT::getInstance().write(errorMsg); \
        CRT::getInstance().write("\n"); \
        x86asmCli(); \
        while (1) \
            ; \
    }
