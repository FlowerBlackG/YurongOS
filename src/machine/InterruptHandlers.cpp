/*


    created on 2022.11.29

*/

#include <yros/machine/InterruptHandlers.h>

#include <CRT.h>
#include <yros/IO.h>
#include <yros/machine/Machine.h>
#include <string.h>
#include <stdio.h>

void* interruptHandlers[InterruptDescriptorTable::DESCRIPTOR_COUNT];

namespace InterruptHandlers {

    void clockInterruptEntrance() {
        x86asmSaveContext(); 
        
        x86asmDirectCall(clockInterruptHandler); 
        x86asmRestoreContext();

        IO::outByte(Machine::PIC_MASTER_CTRL, Machine::PIC_EOI);

        x86asmLeave(); 
        x86asmIret(); 
    }

    void clockInterruptHandler(
        CommonRegisters* commonRegs, 
        ContextRegisters* contextRegs
    ) {
        
        char s[128];
        sprintf(s, "clock interrupt: eax = %d\n", commonRegs->eax);
        CRT::getInstance().write(s);
    }

    void defaultHandler(
        CommonRegisters* commonRegs, 
        ContextRegisters* contextRegs
    ) {
        CRT::getInstance().write("bad exception handler!\n");
        while (1)
            ;
    }

#define IMPLEMENT_EXCEPTION_ENTRANCE(entranceFunctionName, handlerName) \
    void entranceFunctionName() { \
        x86asmSaveContext(); \
        \
        x86asmDirectCall(handlerName); \
        x86asmRestoreContext(); \
        x86asmLeave(); \
        x86asmIret(); \
    }

#define IMPLEMENT_EXCEPTION_WITH_ERRCODE_ENTRANCE(entranceFunctionName, handlerName) \
    void entranceFunctionName() { \
        x86asmSaveContext(); \
        \
        x86asmDirectCall(handlerName); \
        x86asmRestoreContext(); \
        x86asmLeave(); \
        __asm ("addl $4, %esp"); \
        x86asmIret(); \
    }

#undef IMPLEMENT_EXCEPTION_ENTRANCE
#undef IMPLEMENT_EXCEPTION_WITH_ERRCODE_ENTRANCE

}

