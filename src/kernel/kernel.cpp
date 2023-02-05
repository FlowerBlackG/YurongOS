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


/*
 * 手动列出所有需要构造的对象的静态对象。
 */


CRT CRT::instance;
Machine Machine::instance;
MemoryManager MemoryManager::instance;



/**
 * 调用内核所有模块的对象的构造函数。
 */
inline static void callKernelModuleConstructors() {
    extern void (* __CTOR_LIST__)();
    extern void (* __CTOR_END__)();
    
    void (**constructors)() = &__CTOR_LIST__;

    while (constructors != &__CTOR_END__) {
        (*constructors)(); // 调用构造函数。
        ++constructors; // 指向下一个构造函数。
    }
}


inline static void callKernelModuleDestructors() {
    extern void (* __DTOR_LIST__)();
    extern void (* __DTOR_END__)();

    void (**destructors)() = &__DTOR_LIST__;
    
    while (destructors != &__DTOR_END__) {
        (*destructors)();
        ++destructors;
    }
}

/**
 * 内核进入桥。用于连接汇编与C++对象。
 * 加入 extern "C" 以防止 C++ 编译器将函数重命名，导致链接失败。
 */
extern "C" void kernel_bridge() {
    callKernelModuleConstructors();
    Kernel::main();
    callKernelModuleDestructors();
}

void Kernel::panic(const char* s) {
    x86asmCli();

    CRT::getInstance().write(s);

    while (true) {
        x86asmHlt();
    }
}

void Kernel::main() {

    char s[256];

    CRT::getInstance().init();
    CRT::getInstance().write("welcome to yros!\n");   

    Machine::getInstance().init();

    //x86asmSti();

    CRT::getInstance().write("kernel init done.\n");

    while (true) {
        x86asmHlt();
    }
}
