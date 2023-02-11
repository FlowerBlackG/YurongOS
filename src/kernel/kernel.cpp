/*
 * 系统内核。
 * 创建于 2022年7月1日
 */

#include <lib/sys/types.h>
#include <yros/IO.h>
#include <yros/Kernel.h>
#include <yros/CRT.h>
#include <lib/stdarg.h>
#include <lib/stdio.h>
#include <yros/machine/Machine.h>
#include <yros/machine/X86Assembly.h>
#include <yros/memory/MemoryManager.h>
#include <yros/memory/KernelMemoryAllocator.h>
#include <yros/task/TaskManager.h>
#include <yros/interrupt/SystemCall.h>
#include <yros/interrupt/InterruptExit.h>
#include <yros/task/IdleTask.h>
#include <yros/machine/Msr.h>
#include <yros/PerCpuCargo.h>

/**
 * 调用内核所有模块的对象的构造函数。
 */
inline static void __callKernelModuleConstructors() {
    extern void (* __CTOR_LIST__)();
    extern void (* __CTOR_END__)();
    
    void (**constructors)() = &__CTOR_LIST__;

    while (constructors != &__CTOR_END__) {
        (*constructors)(); // 调用构造函数。
        ++constructors; // 指向下一个构造函数。
    }
}


inline static void __callKernelModuleDestructors() {
    extern void (* __DTOR_LIST__)();
    extern void (* __DTOR_END__)();

    void (**destructors)() = &__DTOR_LIST__;
    
    while (destructors != &__DTOR_END__) {
        (*destructors)();
        ++destructors;
    }
}

inline static void __initPerCpuCargos(uint32_t cpuCount) {
    extern uint64_t __PER_CPU_CARGO_START__;
    uint64_t perCpuCargoStart = (uint64_t) &__PER_CPU_CARGO_START__;

    char* pCargo = (char*) perCpuCargoStart;

    for (uint32_t i = 0; i < cpuCount * sizeof(PerCpuCargo); i++) {
        *pCargo = 0;
        pCargo++;
    }
}

inline static void __zeroFillBssArea() {
    extern uint64_t __BSS_START__;
    extern uint64_t __BSS_END__;


    uint64_t bssStart = (uint64_t) &__BSS_START__;
    uint64_t bssEnd = (uint64_t) &__BSS_END__;

    for (uint64_t pos = bssStart; pos < bssEnd; pos++) {
        * ((char*) pos) = 0;
    }

}

/**
 * 内核进入桥。用于连接汇编与C++对象。
 * 加入 extern "C" 以防止 C++ 编译器将函数重命名，导致链接失败。
 */
extern "C" void kernel_bridge() {
    __initPerCpuCargos(1);
    __zeroFillBssArea();
    __callKernelModuleConstructors();
    Kernel::main();
    __callKernelModuleDestructors();
}

void Kernel::panic(const char* s) {
    x86asmCli();

    CRT::getInstance().write(s);

    while (true) {
        x86asmHlt();
    }
}

void write_num(int x) {
    x++;
    x--;
    while (true) {
    //    x86asmSyscall();
            
        char s[128];
        
    }
}

void t1() {
    for (int i = 0; i < 20000000; i++)
            ;
    write_num(1);
    
}


void t2() {                    
    for (int i = 0; i < 50000000; i++)
            ;
    
    write_num(2);
}


void t0() {
     for (int i = 0; i < 1000; i++)
            ;
    write_num(0);
}

void Kernel::main() {

    char s[256];

    CRT::getInstance().init();
    CRT::getInstance().write("welcome to yros!\n");   

    Machine::getInstance().init();
    TaskManager::init();
    SystemCall::init();

    Msr::write(Msr::KERNEL_GS_BASE, (uint64_t) perCpuCargo);

    Task* idleTask = TaskManager::create(
        IdleTask::entrance, 
        "kerneld", 
        false, 
        true
    );


    TaskManager::create(t1, "p1");
   // TaskManager::create(t2, "p2", true);


    // 启动 idle 进程。

    __asm (
        "movq %%rax, %%rsp \n\t"
        :
        : "a" (idleTask->kernelStackPointer)
    );
    
    x86asmSwapgs();
    x86asmSti();
    x86asmNearJmp(interruptExit);

    x86asmUd2();
}
