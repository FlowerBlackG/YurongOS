// SPDX-License-Identifier: MulanPSL-2.0


/*

    键盘中断

    创建于2023年1月21日 江西省上饶市玉山县

*/

#include <interrupt/KeyboardInterrupt.h>
#include <interrupt/ImplementHandlerMacros.h>
#include <crt/CRT.h>
#include <machine/Machine.h>
#include <misc/io.h>
#include <lib/stdio.h>
#include <lib/AsciiChar.h>
#include <machine/apic/apic.h>

#include <task/TaskManager.h>

namespace KeyboardInterrupt {

    uint8_t keymap[KEYMAP_SIZE] = {
        0, AsciiChar::ESC, '1', '2', '3', '4', '5', '6', '7', '8', // [0, 9]
        '9', '0', '-', '=', KEY_BACKSPACE, AsciiChar::TAB, 'q', 'w', 'e', 'r', // [10, 19]
        't', 'y', 'u', 'i', 'o', 'p', '[', ']', KEY_ENTER, KEY_CTRL, // [20, 29]
        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', // [30, 39]
        '\'', '`', KEY_LSHIFT, '\\', 'z', 'x', 'c', 'v', 'b', 'n', // [40, 49]
        'm', ',', '.', '/', KEY_RSHIFT, 0, 0, ' ', 0, 0 // [50, 59]
    };

    IMPLEMENT_EXCEPTION_ENTRANCE(entrance, KeyboardInterrupt::handler)

    void handler(
        InterruptSoftwareFrame* softwareRegs, 
        InterruptHardwareFrame* hardwareRegs
    ) {

        
        auto&& status = io::inByte(KeyboardInterrupt::STATUS_PORT);
        auto&& scancode = io::inByte(KeyboardInterrupt::DATA_PORT);

        char s[128];
        sprintf(s, "status: %d\ndata: %d\n", status, scancode);
        

        uint8_t key = keymap[scancode];

        if (key >= AsciiChar::START_OF_VISIBLE_CHARS && key <= AsciiChar::END_OF_VISIBLE_CHARS) {
        //    CRT::getInstance().putchar(key);
        } else {
        //    CRT::getInstance().write("------------\nkeyboard interrupt\n");
        //    CRT::getInstance().write(s);
        }


        /* 通知中断控制器，该中断处理完毕。 */
        io::outByte(Machine::PIC_MASTER_CTRL, Machine::PIC_EOI);
        machine::apic::localApicEOI();

        // TaskManager::schedule(); 感觉没必要切换任务。
    }
}
