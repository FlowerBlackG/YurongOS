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
#include <memory/KernelMemoryAllocator.h>


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

    x86asmCli();

    s[0] = '\0';

    while (true) {
        void* mem = nullptr;


        for (long long i = 0; i < 14LL * 1024 * 1024 * 1024; i+=1024000LL) {

            int mega = (i / 1024 / 1024) % 1024;
            int giga = (i / 1024 / 1024 / 1024) % 1024;
            int kilo = (i / 1024) % 1024;

            if (i == 4081) {
                int x = 1;
                mem = mem + 1;
                mem = mem - 1;
            }

            mem = KernelMemoryAllocator::malloc(i);
            sprintf(s, "mem alloc: %d G %d M %d K at 0x%llx ", giga, mega, kilo, mem);
            CRT::getInstance().write(s);

            if (giga == 3 && mega == 1023 && kilo == 1020) {
                giga = 3;
            }

            KernelMemoryAllocator::free(mem);
            CRT::getInstance().write("[freed]\n");
        }

    }

    CRT::getInstance().write("mem alloc test done.\n");

    while (true) {
        x86asmHlt();
    }
}
