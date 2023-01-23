/*
 * 系统内核。
 * 创建于 2022年7月1日
 */

#include <sys/types.h>
#include <yros/IO.h>
#include <yros/Kernel.h>
#include <CRT.h>
#include <stdarg.h>
#include <stdio.h>
#include <machine/Machine.h>
#include <machine/X86Assembly.h>
#include <memory/MemoryManager.h>


/*
 * 手动列出所有需要构造的对象的静态对象。
 * 注意：构造顺序是按照列出的顺序来的。
 */

Kernel Kernel::instance;
CRT CRT::instance;
Machine Machine::instance;
MemoryManager MemoryManager::instance;

/**
 * 内核构造函数。不做任何事情。
 */
Kernel::Kernel() {

}

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
    Kernel::getInstance().main();
    callKernelModuleDestructors();
}

void Kernel::panic(const char* s) {
    x86asmCli();

    CRT::getInstance().write(s);

    while (true)
        ;
}

void Kernel::main() {

    CRT::getInstance().init();
    CRT::getInstance().write("welcome to yros!\n");   

    Machine::getInstance().init();

    x86asmSti();

    Kernel::getInstance().panic("[panic]\n");
}
