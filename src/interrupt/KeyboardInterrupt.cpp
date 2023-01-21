/*

    键盘中断

    创建于2023年1月21日 江西省上饶市玉山县

*/

#include <interrupt/KeyboardInterrupt.h>
#include <CRT.h>
#include <machine/Machine.h>
#include <yros/IO.h>
#include <stdio.h>
#include <AsciiChar.h>

namespace KeyboardInterrupt {

    uint8_t keymap[KEYMAP_SIZE] = {
        0, AsciiChar::ESC, '1', '2', '3', '4', '5', '6', '7', '8', // [0, 9]
        '9', '0', '-', '=', KEY_BACKSPACE, AsciiChar::TAB, 'q', 'w', 'e', 'r', // [10, 19]
        't', 'y', 'u', 'i', 'o', 'p', '[', ']', KEY_ENTER, KEY_CTRL, // [20, 29]
        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', // [30, 39]
        '\'', '`', KEY_LSHIFT, '\\', 'z', 'x', 'c', 'v', 'b', 'n', // [40, 49]
        'm', ',', '.', '/', KEY_RSHIFT, 0, 0, ' ', 0, 0 // [50, 59]
    };

    void entrance() {
        __asm ("pushq $0");
        x86asmSaveContext(); 

        x86asmDirectCall(KeyboardInterrupt::handler); 
        x86asmRestoreContext();

        __asm ("addq $8, %rsp"); \
        
        x86asmLeave(); 
        x86asmIret(); 
    }

    void handler(
        SoftwareContextRegisters* softwareRegs, 
        HardwareContextRegisters* hardwareRegs
    ) {

        

        auto&& status = IO::inByte(KeyboardInterrupt::STATUS_PORT);
        auto&& scancode = IO::inByte(KeyboardInterrupt::DATA_PORT);

        char s[128];
        sprintf(s, "status: %d\ndata: %d\n", status, scancode);
        

        uint8_t key = keymap[scancode];

        if (key >= AsciiChar::START_OF_VISIBLE_CHARS && key <= AsciiChar::END_OF_VISIBLE_CHARS) {
            CRT::getInstance().putchar(key);
        } else if (!(scancode & 0x90)) {
            CRT::getInstance().write("------------\nkeyboard interrupt\n");
            CRT::getInstance().write(s);
        }

        /* 通知中断控制器，该中断处理完毕。 */
        IO::outByte(Machine::PIC_MASTER_CTRL, Machine::PIC_EOI);
    }
}
