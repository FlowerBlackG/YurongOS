/*

    时钟中断

    创建于2023年1月21日 江西省上饶市玉山县

*/


#include <interrupt/ClockInterrupt.h>
#include <CRT.h>
#include <yros/IO.h>
#include <machine/Machine.h>

namespace ClockInterrupt {


    void entrance() {
        __asm ("pushq $0");
        x86asmSaveContext(); 

        x86asmDirectCall(ClockInterrupt::handler); 
        x86asmRestoreContext();

        __asm ("addq $8, %rsp"); \
        
        x86asmLeave(); 
        x86asmIret(); 
    }

    void handler(
        SoftwareContextRegisters* softwareRegs, 
        HardwareContextRegisters* hardwareRegs
    ) {
        
        IO::outByte(Machine::PIC_MASTER_CTRL, Machine::PIC_EOI);
        
    }


}

