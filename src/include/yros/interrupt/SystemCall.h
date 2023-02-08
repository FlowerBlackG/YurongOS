/*

    系统调用

    创建于 2023年2月6日 江西省上饶市玉山县

*/

#pragma once

#include <yros/machine/X86Assembly.h>
#include <lib/sys/types.h>

namespace SystemCall {

    const int SYSCALL_NUM = 128;

    void init();

    void entrance();
    void welcomeMat(
        SoftwareContextRegisters* softwareRegs, 
        HardwareContextRegisters* hardwareRegs
    );

    int64_t test();

}
