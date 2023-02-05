/*

    时钟中断

    创建于2023年1月21日 江西省上饶市玉山县

*/


#include <yros/interrupt/ClockInterrupt.h>
#include <yros/interrupt/ImplementHandlerMacros.h>
#include <yros/CRT.h>
#include <yros/IO.h>
#include <yros/machine/Machine.h>

namespace ClockInterrupt {

    IMPLEMENT_EXCEPTION_ENTRANCE(entrance, ClockInterrupt::handler)

    void handler(
        SoftwareContextRegisters* softwareRegs, 
        HardwareContextRegisters* hardwareRegs
    ) {
        
        IO::outByte(Machine::PIC_MASTER_CTRL, Machine::PIC_EOI);
        
    }


}

