/*


    created on 2022.11.29

*/

#pragma once

#include <machine/InterruptDescriptorTable.h>
#include <machine/X86Assembly.h>
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
        SoftwareContextRegisters* softwareRegs, 
        HardwareContextRegisters* hardwareRegs
    );

    void defaultHandler(
        SoftwareContextRegisters* softwareRegs, 
        HardwareContextRegisters* hardwareRegs
    );

    
    



    void divideErrorExceptionEntrance();
    void divideErrorExceptionHandler(
        SoftwareContextRegisters* softwareRegs,
        HardwareContextRegisters* hardwareRegs
    );

    void debugExceptionEntrance();
    void debugExceptionHandler(
        SoftwareContextRegisters* softwareRegs,
        HardwareContextRegisters* hardwareRegs
    );

    void nonMaskableInterruptExceptionEntrance();
    void nonMaskableInterruptExceptionHandler(
        SoftwareContextRegisters* softwareRegs,
        HardwareContextRegisters* hardwareRegs
    );

    void breakpointExceptionEntrance();
    void breakpointExceptionHandler(
        SoftwareContextRegisters* softwareRegs,
        HardwareContextRegisters* hardwareRegs
    );

    void overflowExceptionEntrance();
    void overflowExceptionHandler(
        SoftwareContextRegisters* softwareRegs,
        HardwareContextRegisters* hardwareRegs
    );

    void boundaryRangeExceededExceptionEntrance();
    void boundaryRangeExceededExceptionHandler(
        SoftwareContextRegisters* softwareRegs,
        HardwareContextRegisters* hardwareRegs
    );

    void undefinedOpcodeExceptionEntrance();
    void undefinedOpcodeExceptionHandler(
        SoftwareContextRegisters* softwareRegs,
        HardwareContextRegisters* hardwareRegs
    );

    void deviceNotAvailableExceptionEntrance();
    void deviceNotAvailableExceptionHandler(
        SoftwareContextRegisters* softwareRegs,
        HardwareContextRegisters* hardwareRegs
    );

    void doubleFaultExceptionEntrance();
    void doubleFaultExceptionHandler(
        SoftwareContextRegisters* softwareRegs,
        HardwareContextRegisters* hardwareRegs
    );

    void invalidTssExceptionEntrance();
    void invalidTssExceptionHandler(
        SoftwareContextRegisters* softwareRegs,
        HardwareContextRegisters* hardwareRegs
    );

    void notPresentExceptionEntrance();
    void notPresentExceptionHandler(
        SoftwareContextRegisters* softwareRegs,
        HardwareContextRegisters* hardwareRegs
    );

    void stackSegmentExceptionEntrance();
    void stackSegmentExceptionHandler(
        SoftwareContextRegisters* softwareRegs,
        HardwareContextRegisters* hardwareRegs
    );

    void generalProtectionExceptionEntrance();
    void generalProtectionExceptionHandler(
        SoftwareContextRegisters* softwareRegs,
        HardwareContextRegisters* hardwareRegs
    );

    void pageFaultExceptionEntrance();
    void pageFaultExceptionHandler(
        SoftwareContextRegisters* softwareRegs,
        HardwareContextRegisters* hardwareRegs
    );

    void mathFaultExceptionEntrance();
    void mathFaultExceptionHandler(
        SoftwareContextRegisters* softwareRegs,
        HardwareContextRegisters* hardwareRegs
    );

    void alignmentCheckingExceptionEntrance();
    void alignmentCheckingExceptionHandler(
        SoftwareContextRegisters* softwareRegs,
        HardwareContextRegisters* hardwareRegs
    );

    void machineCheckExceptionEntrance();
    void machineCheckExceptionHandler(
        SoftwareContextRegisters* softwareRegs,
        HardwareContextRegisters* hardwareRegs
    );

    void extendedMathFaultExceptionEntrance();
    void extendedMathFaultExceptionHandler(
        SoftwareContextRegisters* softwareRegs,
        HardwareContextRegisters* hardwareRegs
    );

    void virtualizationExceptionExceptionEntrance();
    void virtualizationExceptionExceptionHandler(
        SoftwareContextRegisters* softwareRegs,
        HardwareContextRegisters* hardwareRegs
    );

    void controlProtectionExceptionExceptionEntrance();
    void controlProtectionExceptionExceptionHandler(
        SoftwareContextRegisters* softwareRegs,
        HardwareContextRegisters* hardwareRegs
    );
}



