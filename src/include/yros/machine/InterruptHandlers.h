/*


    created on 2022.11.29

*/

#pragma once

#include <yros/machine/InterruptDescriptorTable.h>
#include <yros/machine/X86Assembly.h>
#include <sys/types.h>

extern void* interruptHandlers[InterruptDescriptorTable::DESCRIPTOR_COUNT];

extern void* interrupt_handler_bridges[];

namespace InterruptHandlers {

    const int OSCILLATOR_FREQUENCY = 1193182;
    const int WANTED_CLOCK_INTERRUPT_HZ = 100;
    const int CLOCK_COUNTER = OSCILLATOR_FREQUENCY / WANTED_CLOCK_INTERRUPT_HZ;
    const int JIFFY = 1000 / WANTED_CLOCK_INTERRUPT_HZ;

    void clockInterruptEntrance();

    void clockInterruptHandler(
        CommonRegisters* commonRegs, 
        ContextRegisters* contextRegs
    );

    void defaultHandler(
        CommonRegisters* commonRegs, 
        ContextRegisters* contextRegs
    );


}



