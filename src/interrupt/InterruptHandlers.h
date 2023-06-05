/*


    created on 2022.11.29

*/

#pragma once

#include <interrupt/InterruptDescriptorTable.h>
#include <machine/X86Assembly.h>
#include <lib/sys/types.h>

namespace InterruptHandlers {

    void defaultHandler(
        InterruptSoftwareFrame* softwareRegs,
        InterruptHardwareFrame* hardwareRegs
    );


    void divideErrorExceptionEntrance();
    void divideErrorExceptionHandler(
        InterruptSoftwareFrame* softwareRegs,
        InterruptHardwareFrame* hardwareRegs
    );

    void debugExceptionEntrance();
    void debugExceptionHandler(
        InterruptSoftwareFrame* softwareRegs,
        InterruptHardwareFrame* hardwareRegs
    );

    void nonMaskableInterruptExceptionEntrance();
    void nonMaskableInterruptExceptionHandler(
        InterruptSoftwareFrame* softwareRegs,
        InterruptHardwareFrame* hardwareRegs
    );

    void breakpointExceptionEntrance();
    void breakpointExceptionHandler(
        InterruptSoftwareFrame* softwareRegs,
        InterruptHardwareFrame* hardwareRegs
    );

    void overflowExceptionEntrance();
    void overflowExceptionHandler(
        InterruptSoftwareFrame* softwareRegs,
        InterruptHardwareFrame* hardwareRegs
    );

    void boundaryRangeExceededExceptionEntrance();
    void boundaryRangeExceededExceptionHandler(
        InterruptSoftwareFrame* softwareRegs,
        InterruptHardwareFrame* hardwareRegs
    );

    void undefinedOpcodeExceptionEntrance();
    void undefinedOpcodeExceptionHandler(
        InterruptSoftwareFrame* softwareRegs,
        InterruptHardwareFrame* hardwareRegs
    );

    void deviceNotAvailableExceptionEntrance();
    void deviceNotAvailableExceptionHandler(
        InterruptSoftwareFrame* softwareRegs,
        InterruptHardwareFrame* hardwareRegs
    );

    void doubleFaultExceptionEntrance();
    void doubleFaultExceptionHandler(
        InterruptSoftwareFrame* softwareRegs,
        InterruptHardwareFrame* hardwareRegs
    );

    void invalidTssExceptionEntrance();
    void invalidTssExceptionHandler(
        InterruptSoftwareFrame* softwareRegs,
        InterruptHardwareFrame* hardwareRegs
    );

    void notPresentExceptionEntrance();
    void notPresentExceptionHandler(
        InterruptSoftwareFrame* softwareRegs,
        InterruptHardwareFrame* hardwareRegs
    );

    void stackSegmentExceptionEntrance();
    void stackSegmentExceptionHandler(
        InterruptSoftwareFrame* softwareRegs,
        InterruptHardwareFrame* hardwareRegs
    );

    void generalProtectionExceptionEntrance();
    void generalProtectionExceptionHandler(
        InterruptSoftwareFrame* softwareRegs,
        InterruptHardwareFrame* hardwareRegs
    );

    // page fault 处理方式见 interrupt/PageFaultException.h

    void mathFaultExceptionEntrance();
    void mathFaultExceptionHandler(
        InterruptSoftwareFrame* softwareRegs,
        InterruptHardwareFrame* hardwareRegs
    );

    void alignmentCheckingExceptionEntrance();
    void alignmentCheckingExceptionHandler(
        InterruptSoftwareFrame* softwareRegs,
        InterruptHardwareFrame* hardwareRegs
    );

    void machineCheckExceptionEntrance();
    void machineCheckExceptionHandler(
        InterruptSoftwareFrame* softwareRegs,
        InterruptHardwareFrame* hardwareRegs
    );

    void extendedMathFaultExceptionEntrance();
    void extendedMathFaultExceptionHandler(
        InterruptSoftwareFrame* softwareRegs,
        InterruptHardwareFrame* hardwareRegs
    );

    void virtualizationExceptionExceptionEntrance();
    void virtualizationExceptionExceptionHandler(
        InterruptSoftwareFrame* softwareRegs,
        InterruptHardwareFrame* hardwareRegs
    );

    void controlProtectionExceptionExceptionEntrance();
    void controlProtectionExceptionExceptionHandler(
        InterruptSoftwareFrame* softwareRegs,
        InterruptHardwareFrame* hardwareRegs
    );
}



