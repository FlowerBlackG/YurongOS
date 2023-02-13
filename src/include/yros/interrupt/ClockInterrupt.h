/*

    时钟中断

    创建于2023年1月21日 江西省上饶市玉山县

*/

#pragma once 

#include <yros/machine/X86Assembly.h>
#include <lib/sys/types.h>

/**
 * 时钟中断。
 */
namespace ClockInterrupt {

    /**
     * 震荡频率。
     */
    const int OSCILLATOR_FREQUENCY = 1193182;

    /**
     * 系统希望的时钟频率。
     */
    const int WANTED_CLOCK_INTERRUPT_HZ = 100;
    
    const int CLOCK_COUNTER = OSCILLATOR_FREQUENCY / WANTED_CLOCK_INTERRUPT_HZ;

    /**
     * 每时间片的毫秒数。
     */
    const int JIFFY = 1000 / WANTED_CLOCK_INTERRUPT_HZ;

    extern uint64_t jiffyCounter;

    void entrance();

    void handler(
        InterruptSoftwareFrame* softwareRegs, 
        InterruptHardwareFrame* hardwareRegs
    );

}
