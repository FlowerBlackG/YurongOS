/*

    时钟中断

    创建于2023年1月21日 江西省上饶市玉山县

*/


#include <interrupt/ClockInterrupt.h>
#include <interrupt/ImplementHandlerMacros.h>
#include <CRT.h>
#include <yros/IO.h>
#include <machine/Machine.h>

namespace ClockInterrupt {

    IMPLEMENT_EXCEPTION_ENTRANCE(entrance, ClockInterrupt::handler)

    void handler(
        SoftwareContextRegisters* softwareRegs, 
        HardwareContextRegisters* hardwareRegs
    ) {
        
        IO::outByte(Machine::PIC_MASTER_CTRL, Machine::PIC_EOI);
        
    }


}

