/*


    created on 2022.11.29

*/

#include <yros/interrupt/InterruptHandlers.h>
#include <yros/interrupt/ImplementHandlerMacros.h>

#include <yros/CRT.h>
#include <yros/IO.h>
#include <yros/machine/Machine.h>
#include <lib/string.h>
#include <lib/stdio.h>

namespace InterruptHandlers {

    void defaultHandler(
        SoftwareContextRegisters* softwareRegs, 
        HardwareContextRegisters* hardwareRegs
    ) {
        CRT::getInstance().write("bad exception handler!\n");
        while (1)
            ;
    }

    IMPLEMENT_EXCEPTION_ENTRANCE(divideErrorExceptionEntrance, divideErrorExceptionHandler)
    IMPLEMENT_EXCEPTION_HANDLER(divideErrorExceptionHandler, "divideError", 0)
    IMPLEMENT_EXCEPTION_ENTRANCE(debugExceptionEntrance, debugExceptionHandler)
    IMPLEMENT_EXCEPTION_HANDLER(debugExceptionHandler, "debug", 0)
    IMPLEMENT_EXCEPTION_ENTRANCE(nonMaskableInterruptExceptionEntrance, nonMaskableInterruptExceptionHandler)
    IMPLEMENT_EXCEPTION_HANDLER(nonMaskableInterruptExceptionHandler, "nonMaskableInterrupt", 0)
    IMPLEMENT_EXCEPTION_ENTRANCE(breakpointExceptionEntrance, breakpointExceptionHandler)
    IMPLEMENT_EXCEPTION_HANDLER(breakpointExceptionHandler, "breakpoint", 0)
    IMPLEMENT_EXCEPTION_ENTRANCE(overflowExceptionEntrance, overflowExceptionHandler)
    IMPLEMENT_EXCEPTION_HANDLER(overflowExceptionHandler, "overflow", 0)
    IMPLEMENT_EXCEPTION_ENTRANCE(boundaryRangeExceededExceptionEntrance, boundaryRangeExceededExceptionHandler)
    IMPLEMENT_EXCEPTION_HANDLER(boundaryRangeExceededExceptionHandler, "boundaryRangeExceeded", 0)
    IMPLEMENT_EXCEPTION_ENTRANCE(undefinedOpcodeExceptionEntrance, undefinedOpcodeExceptionHandler)
    IMPLEMENT_EXCEPTION_HANDLER(undefinedOpcodeExceptionHandler, "undefinedOpcode", 0)
    IMPLEMENT_EXCEPTION_ENTRANCE(deviceNotAvailableExceptionEntrance, deviceNotAvailableExceptionHandler)
    IMPLEMENT_EXCEPTION_HANDLER(deviceNotAvailableExceptionHandler, "deviceNotAvailable", 0)
    IMPLEMENT_EXCEPTION_WITH_ERRCODE_ENTRANCE(doubleFaultExceptionEntrance, doubleFaultExceptionHandler)
    IMPLEMENT_EXCEPTION_WITH_ERRCODE_HANDLER(doubleFaultExceptionHandler, "doubleFault", 0)
    IMPLEMENT_EXCEPTION_WITH_ERRCODE_ENTRANCE(invalidTssExceptionEntrance, invalidTssExceptionHandler)
    IMPLEMENT_EXCEPTION_WITH_ERRCODE_HANDLER(invalidTssExceptionHandler, "invalidTss", 0)
    IMPLEMENT_EXCEPTION_WITH_ERRCODE_ENTRANCE(notPresentExceptionEntrance, notPresentExceptionHandler)
    IMPLEMENT_EXCEPTION_WITH_ERRCODE_HANDLER(notPresentExceptionHandler, "notPresent", 0)
    IMPLEMENT_EXCEPTION_WITH_ERRCODE_ENTRANCE(stackSegmentExceptionEntrance, stackSegmentExceptionHandler)
    IMPLEMENT_EXCEPTION_WITH_ERRCODE_HANDLER(stackSegmentExceptionHandler, "stackSegment", 0)
    IMPLEMENT_EXCEPTION_WITH_ERRCODE_ENTRANCE(generalProtectionExceptionEntrance, generalProtectionExceptionHandler)
    IMPLEMENT_EXCEPTION_WITH_ERRCODE_HANDLER(generalProtectionExceptionHandler, "generalProtection", 0)
    IMPLEMENT_EXCEPTION_WITH_ERRCODE_ENTRANCE(pageFaultExceptionEntrance, pageFaultExceptionHandler)
    IMPLEMENT_EXCEPTION_WITH_ERRCODE_HANDLER(pageFaultExceptionHandler, "pageFault", 0)
    IMPLEMENT_EXCEPTION_ENTRANCE(mathFaultExceptionEntrance, mathFaultExceptionHandler)
    IMPLEMENT_EXCEPTION_HANDLER(mathFaultExceptionHandler, "mathFault", 0)
    IMPLEMENT_EXCEPTION_WITH_ERRCODE_ENTRANCE(alignmentCheckingExceptionEntrance, alignmentCheckingExceptionHandler)
    IMPLEMENT_EXCEPTION_WITH_ERRCODE_HANDLER(alignmentCheckingExceptionHandler, "alignmentChecking", 0)
    IMPLEMENT_EXCEPTION_ENTRANCE(machineCheckExceptionEntrance, machineCheckExceptionHandler)
    IMPLEMENT_EXCEPTION_HANDLER(machineCheckExceptionHandler, "machineCheck", 0)
    IMPLEMENT_EXCEPTION_ENTRANCE(extendedMathFaultExceptionEntrance, extendedMathFaultExceptionHandler)
    IMPLEMENT_EXCEPTION_HANDLER(extendedMathFaultExceptionHandler, "extendedMathFault", 0)
    IMPLEMENT_EXCEPTION_ENTRANCE(virtualizationExceptionExceptionEntrance, virtualizationExceptionExceptionHandler)
    IMPLEMENT_EXCEPTION_HANDLER(virtualizationExceptionExceptionHandler, "virtualizationException", 0)
    IMPLEMENT_EXCEPTION_WITH_ERRCODE_ENTRANCE(controlProtectionExceptionExceptionEntrance, controlProtectionExceptionExceptionHandler)
    IMPLEMENT_EXCEPTION_WITH_ERRCODE_HANDLER(controlProtectionExceptionExceptionHandler, "controlProtectionException", 0)

    /*
    
        x86 异常列表：

        divide error
        debug
        nonMaskable interrupt
        breakpoint
        overflow
        boundary range exceeded
        undefined opcode
        device not available
        double fault
        --
        invalid tss
        not present
        stack segment
        general protection
        page fault
        --
        math fault
        alignment checking
        machine check
        extended math fault
        virtualization exception
        control protection exception

    
    */

#undef IMPLEMENT_EXCEPTION_HANDLER
#undef IMPLEMENT_EXCEPTION_WITH_ERRCODE_HANDLER
#undef IMPLEMENT_EXCEPTION_ENTRANCE
#undef IMPLEMENT_EXCEPTION_WITH_ERRCODE_ENTRANCE

}


