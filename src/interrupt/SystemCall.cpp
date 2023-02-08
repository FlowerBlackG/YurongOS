/*

    系统调用

    创建于 2023年2月6日 江西省上饶市玉山县

*/

#include <yros/interrupt/SystemCall.h>
#include <yros/interrupt/ImplementHandlerMacros.h>

namespace SystemCall {

    struct {

        int paramCount;
        int64_t (* handler) ();
    
    } handlers[SYSCALL_NUM] = {
        
        { 0, test },
        { 0, test }
    
    };

    void init() {
        
    }

    IMPLEMENT_EXCEPTION_ENTRANCE(entrance, SystemCall::welcomeMat)

    void welcomeMat(
        SoftwareContextRegisters* softwareRegs, 
        HardwareContextRegisters* hardwareRegs
    ) { // todo
        handlers[softwareRegs->rax].handler();
    }

    int64_t test() {
        return 0;
    }

}
