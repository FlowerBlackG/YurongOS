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
    void initGdt();

    /**
     * 初始化中断描述符表。
     */
    void initIdt();

    /**
     * 初始化 8259A 外中断控制芯片。
     */
    void initPic();

    /**
     * 初始化 8253 芯片。
     * Programmable Interval Timer.
     */
    void initPit();


public:
    static const int PIC_MASTER_CTRL = 0x20;
    static const int PIC_MASTER_DATA = 0x21;
    static const int PIC_SLAVE_CTRL = 0xa0;
    static const int PIC_SLAVE_DATA = 0xa1;
    static const int PIC_EOI = 0x20;

private:
    GlobalDescriptorTable gdt;
    GdtRegister gdtr;

    IdtRegister idtr;
    InterruptDescriptorTable idt;

private:
    Machine();
    static Machine instance;
};
