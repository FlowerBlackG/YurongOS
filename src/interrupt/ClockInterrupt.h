// SPDX-License-Identifier: MulanPSL-2.0

/*

    时钟中断

    创建于2023年1月21日 江西省上饶市玉山县

*/

#pragma once 

#include <machine/X86Assembly.h>
#include <lib/sys/types.h>

struct Task;

/**
 * 时钟中断。
 */
namespace ClockInterrupt {

/**
 * 震荡频率。
 * 
 * @deprecated 该值是 8254 芯片使用的。现在时钟使用 hpet 产生。
 */
const int OSCILLATOR_FREQUENCY = 1193182;

/**
 * 系统希望的时钟频率。
 */
const int WANTED_CLOCK_INTERRUPT_HZ = 100;

/**
 * 
 * 
 * @deprecated 该值是 8254 芯片使用的。现在时钟使用 hpet 产生。
 */
const int CLOCK_COUNTER = OSCILLATOR_FREQUENCY / WANTED_CLOCK_INTERRUPT_HZ;

/**
 * 每时间片的毫秒数。
 */
const int JIFFY = 1000 / WANTED_CLOCK_INTERRUPT_HZ;

extern int64_t interruptFrequency;

extern uint64_t jiffyCounter;

/**
 * 从启动算起，经历的时间。单位为毫秒。
 */
extern uint64_t timePassedSinceBoot;

void entrance();

void handler(
    InterruptSoftwareFrame* softwareRegs,
    InterruptHardwareFrame* hardwareRegs
);


void putToSleep(Task* task, uint64_t milliseconds);

}
