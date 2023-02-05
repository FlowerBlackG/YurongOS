/*


    created on 2022.11.29

*/

#pragma once

#include <yros/interrupt/InterruptDescriptorTable.h>
#include <yros/machine/X86Assembly.h>
#include <lib/sys/types.h>

namespace InterruptHandlers {

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



