/*

    created on 2022.11.28
*/


#pragma once

class X86Assembly {

public:

    inline static void sti() {
        __asm ("sti");
    }

    inline static void cli() {
        __asm ("cli");
    }

    inline static void leave() {
        __asm ("leave");
    }
    
    inline static void iret() {
        __asm ("iret");
    }

private:
    X86Assembly() {};

};
