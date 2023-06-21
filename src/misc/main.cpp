/*
 * 系统内核。
 * 创建于 2022年7月1日
 */

#include <lib/sys/types.h>
#include <misc/IO.h>
#include <misc/Kernel.h>
#include <crt/CRT.h>
#include <lib/stdarg.h>
#include <lib/stdio.h>
#include <lib/string.h>
#include <machine/Machine.h>
#include <machine/X86Assembly.h>
#include <memory/MemoryManager.h>
#include <memory/KernelMemoryAllocator.h>
#include <task/TaskManager.h>
#include <interrupt/SystemCall.h>
#include <interrupt/InterruptExit.h>
#include <task/IdleTask.h>
#include <machine/Msr.h>
#include <misc/PerCpuCargo.h>
#include <lib/stddef.h>

#include <lib/syscalls.h>

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

    PerCpuCargo* p = (PerCpuCargo*) perCpuCargoStart;
    for (uint32_t i = 0; i < cpuCount; i++) {
        p->self = p;
        p++;
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

void userApp1() {

    char s[128] = "this is the first user app\n";
    auto len = strlen(s);
    //auto res = write(1, s, len);
    //sprintf(s, "[user app 1] return value: %lld\n", res);
    //write(1, s, strlen(s));

    int counter = 0;



    //testCall();


    while (true) {
        sprintf(s, "[app 1] [%d] sleep...\n", counter);
        write(1, s, strlen(s));

        sleep(2000);

        counter++;
    }
}

void userApp2() {

    char s[192] =
        " _   _ _ __ ___  ___ \n"
        "| | | | '__/ _ \\/ __|\n"
        "| |_| | | | (_) \\__ \\\n"
        " \\__, |_|  \\___/|___/\n"
        "  __/ |              \n"
        " |___/               \n";

 
    auto res = write(1, s, strlen(s));
    sprintf(s, "[user app 2] return value: %lld\n", res);
    write(1, s, strlen(s));

    int counter = 0;

    sleep(1000);

    while (true) {
        int written = 0;
        while (true) {

            int testRes = 0;//testCall();

            sprintf(s, "[app 2] [%d] sleep... (%x) (%d)\n", counter, testRes, written);
            written = write(1, s, strlen(s));

            sleep(2000);

            //fork();

            counter++;
        }
    }
        
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



    TaskManager::create(userApp1, "ua1");
    TaskManager::create(userApp2, "ua2");
   // TaskManager::create(t2, "p2", true);


    // 启动 idle 进程。

    x86asmSwapgs();

    TaskManager::loadTaskToCargo(idleTask);

    __asm (
        "movq %0, %%rsp \n\t"
        :
        : "r" (idleTask->kernelStackPointer)
    );
    
    x86asmSti();
    x86asmNearJmp(interruptExit);

    x86asmUd2();
}