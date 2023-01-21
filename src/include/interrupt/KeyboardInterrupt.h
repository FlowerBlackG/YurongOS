/*

    键盘中断

    创建于2023年1月21日 江西省上饶市玉山县

*/


#pragma once

#include <machine/X86Assembly.h>
#include <sys/types.h>


namespace KeyboardInterrupt {

    const int DATA_PORT = 0x60;
    const int STATUS_PORT = 0x64;

    const int KEYMAP_SIZE = 80;

    const uint8_t KEY_BACKSPACE = 181;
    const uint8_t KEY_ENTER = 182;
    const uint8_t KEY_CTRL = 183;
    const uint8_t KEY_ALT = 184;
    const uint8_t KEY_LSHIFT = 185;
    const uint8_t KEY_RSHIFT = 186;
    

    extern uint8_t keymap[KEYMAP_SIZE];

    void entrance();

    void handler(
        SoftwareContextRegisters* softwareRegs, 
        HardwareContextRegisters* hardwareRegs
    );
}