/*
 * 硬件控制。
 * 创建于 2022年7月15日。
 */

#pragma once

#include <yros/machine/GlobalDescriptorTable.h>
#include <yros/machine/InterruptDescriptorTable.h>

class Machine {
public:
    static Machine& getInstance() {
        return instance;
    }

    void init();

private:
    void initGDT();
    void initIDT();

private:
    GlobalDescriptorTable gdt;
    GdtRegister gdtr;

    IdtRegister idtr;
    InterruptDescriptorTable idt;

private:
    Machine();
    static Machine instance;
};
