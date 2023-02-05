/*

    时钟中断

    创建于2023年1月21日 江西省上饶市玉山县

*/

#pragma once 

#include <yros/machine/X86Assembly.h>
#include <lib/sys/types.h>


namespace ClockInterrupt {
    const int OSCILLATOR_FREQUENCY = 1193182;
    const int WANTED_CLOCK_INTERRUPT_HZ = 100;
    const int CLOCK_COUNTER = OSCILLATOR_FREQUENCY / WANTED_CLOCK_INTERRUPT_HZ;
    const int JIFFY = 1000 / WANTED_CLOCK_INTERRUPT_HZ;

    void entrance();

    void handler(
        SoftwareContextRegisters* softwareRegs, 
        HardwareContextRegisters* hardwareRegs
    );

}
